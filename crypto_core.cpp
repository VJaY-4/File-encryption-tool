#include "crypto_core.h"

#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#include <bcrypt.h>
#endif

namespace fs = std::filesystem;

const std::string ENC_TEXT_DIR = "Encrypted Files/Text Files";
const std::string ENC_IMG_DIR  = "Encrypted Files/Images";
const std::string DEC_TEXT_DIR = "Decrypted Files/Text Files";
const std::string DEC_IMG_DIR  = "Decrypted Files/Images";
const std::string INPUT_DIR    = "Input Files";

void ensureDirectories() {
    fs::create_directories(ENC_TEXT_DIR);
    fs::create_directories(ENC_IMG_DIR);
    fs::create_directories(DEC_TEXT_DIR);
    fs::create_directories(DEC_IMG_DIR);
    fs::create_directories(INPUT_DIR);
}

void xorCipher(char* buf, size_t len, const std::string& key) {
    if (key.empty()) return;
    size_t ksz = key.size();
    for (size_t i = 0; i < len; ++i)
        buf[i] ^= key[i % ksz];
}

void trimTrailing(std::string& s) {
    while (!s.empty() && (s.back() == ' ' || s.back() == '\r' || s.back() == '\n'))
        s.pop_back();
}

static unsigned int keyToSeed(const std::string& key) {
    unsigned int seed = 5381;
    for (unsigned char c : key) seed = seed * 33 + c;
    return seed;
}

static unsigned int lcgNext(unsigned int& state) {
    state = state * 1103515245 + 12345;
    return (state >> 16) & 0x7FFF;
}

static void scrambleBytes(char* buf, size_t len, const std::string& key) {
    if (len <= 1) return;
    unsigned int state = keyToSeed(key);
    size_t n = std::min(len, static_cast<size_t>(300000));
    for (size_t s = 0; s < n; ++s)
        std::swap(buf[lcgNext(state) % len], buf[lcgNext(state) % len]);
}

static void unscrambleBytes(char* buf, size_t len, const std::string& key) {
    if (len <= 1) return;
    unsigned int state = keyToSeed(key);
    size_t n = std::min(len, static_cast<size_t>(300000));
    std::vector<std::pair<size_t, size_t>> swaps(n);
    for (auto& sw : swaps) { sw.first = lcgNext(state) % len; sw.second = lcgNext(state) % len; }
    for (size_t s = n; s-- > 0;) std::swap(buf[swaps[s].first], buf[swaps[s].second]);
}

static std::vector<char> readBinaryFile(const std::string& path) {
    std::ifstream in(path, std::ios::binary | std::ios::ate);
    if (!in) throw std::runtime_error("Cannot open: " + path);
    auto sz = static_cast<size_t>(in.tellg());
    if (sz == 0) throw std::runtime_error("File is empty: " + path);
    in.seekg(0);
    std::vector<char> buf(sz);
    in.read(buf.data(), static_cast<std::streamsize>(sz));
    return buf;
}

static void writeBinaryFile(const std::string& path, const std::vector<char>& buf) {
    std::ofstream out(path, std::ios::binary);
    if (!out) throw std::runtime_error("Cannot create: " + path);
    out.write(buf.data(), static_cast<std::streamsize>(buf.size()));
    if (out.fail()) throw std::runtime_error("Write error: " + path);
}

// ── AES-256-CBC via Windows BCrypt ──────────────────────────────────
#ifdef _WIN32

static std::vector<unsigned char> deriveKey256(const std::string& password) {
    BCRYPT_ALG_HANDLE hAlg = nullptr;
    BCRYPT_HASH_HANDLE hHash = nullptr;
    std::vector<unsigned char> hash(32);

    if (BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA256_ALGORITHM, nullptr, 0) != 0)
        throw std::runtime_error("Failed to init SHA-256.");
    if (BCryptCreateHash(hAlg, &hHash, nullptr, 0, nullptr, 0, 0) != 0) {
        BCryptCloseAlgorithmProvider(hAlg, 0);
        throw std::runtime_error("Failed to create hash.");
    }
    BCryptHashData(hHash, (PUCHAR)password.data(), (ULONG)password.size(), 0);
    BCryptFinishHash(hHash, hash.data(), 32, 0);
    BCryptDestroyHash(hHash);
    BCryptCloseAlgorithmProvider(hAlg, 0);
    return hash;
}

static std::vector<char> aesEncryptBuffer(const std::vector<char>& plain, const std::string& key) {
    auto dk = deriveKey256(key);

    BCRYPT_ALG_HANDLE hAlg = nullptr;
    BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_AES_ALGORITHM, nullptr, 0);

    BCRYPT_KEY_HANDLE hKey = nullptr;
    BCryptGenerateSymmetricKey(hAlg, &hKey, nullptr, 0, dk.data(), 32, 0);

    unsigned char iv[16];
    BCryptGenRandom(nullptr, iv, 16, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
    unsigned char ivCopy[16];
    memcpy(ivCopy, iv, 16);

    ULONG cbOut = 0;
    BCryptEncrypt(hKey, (PUCHAR)plain.data(), (ULONG)plain.size(),
                  nullptr, ivCopy, 16, nullptr, 0, &cbOut, BCRYPT_BLOCK_PADDING);

    memcpy(ivCopy, iv, 16);
    std::vector<unsigned char> cipher(cbOut);
    NTSTATUS st = BCryptEncrypt(hKey, (PUCHAR)plain.data(), (ULONG)plain.size(),
                                nullptr, ivCopy, 16, cipher.data(), cbOut, &cbOut, BCRYPT_BLOCK_PADDING);
    BCryptDestroyKey(hKey);
    BCryptCloseAlgorithmProvider(hAlg, 0);
    if (st != 0) throw std::runtime_error("AES encryption failed.");

    // Output: "AES\0" (4) + IV (16) + ciphertext
    std::vector<char> result;
    result.reserve(4 + 16 + cbOut);
    result.push_back('A'); result.push_back('E'); result.push_back('S'); result.push_back('\0');
    result.insert(result.end(), reinterpret_cast<char*>(iv), reinterpret_cast<char*>(iv) + 16);
    result.insert(result.end(), reinterpret_cast<char*>(cipher.data()),
                  reinterpret_cast<char*>(cipher.data()) + cbOut);
    return result;
}

static std::vector<char> aesDecryptBuffer(const std::vector<char>& data, const std::string& key) {
    if (data.size() < 20 || data[0] != 'A' || data[1] != 'E' || data[2] != 'S' || data[3] != '\0')
        throw std::runtime_error("Not a valid AES-encrypted file.");

    auto dk = deriveKey256(key);
    unsigned char iv[16];
    memcpy(iv, data.data() + 4, 16);

    BCRYPT_ALG_HANDLE hAlg = nullptr;
    BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_AES_ALGORITHM, nullptr, 0);

    BCRYPT_KEY_HANDLE hKey = nullptr;
    BCryptGenerateSymmetricKey(hAlg, &hKey, nullptr, 0, dk.data(), 32, 0);

    const unsigned char* cipherData = reinterpret_cast<const unsigned char*>(data.data()) + 20;
    ULONG cipherLen = static_cast<ULONG>(data.size() - 20);

    ULONG cbOut = 0;
    unsigned char ivCopy[16];
    memcpy(ivCopy, iv, 16);
    BCryptDecrypt(hKey, const_cast<PUCHAR>(cipherData), cipherLen,
                  nullptr, ivCopy, 16, nullptr, 0, &cbOut, BCRYPT_BLOCK_PADDING);

    memcpy(ivCopy, iv, 16);
    std::vector<unsigned char> plain(cbOut);
    NTSTATUS st = BCryptDecrypt(hKey, const_cast<PUCHAR>(cipherData), cipherLen,
                                nullptr, ivCopy, 16, plain.data(), cbOut, &cbOut, BCRYPT_BLOCK_PADDING);
    BCryptDestroyKey(hKey);
    BCryptCloseAlgorithmProvider(hAlg, 0);
    if (st != 0) throw std::runtime_error("AES decryption failed. Wrong key or corrupted file.");

    return std::vector<char>(plain.begin(), plain.begin() + cbOut);
}

#endif // _WIN32

std::string encryptTextFromString(const std::string& content,
                                  const std::string& filename,
                                  const std::string& key,
                                  CipherMethod method,
                                  std::atomic<float>* progress) {
    if (content.empty()) throw std::runtime_error("Content is empty.");
    if (key.empty())     throw std::runtime_error("Key is empty.");
    if (progress) *progress = 0.2f;
    std::vector<char> buf(content.begin(), content.end());
    if (progress) *progress = 0.4f;
    if (method == CipherMethod::AES256) {
#ifdef _WIN32
        buf = aesEncryptBuffer(buf, key);
#else
        throw std::runtime_error("AES-256 not supported on this platform.");
#endif
    } else {
        xorCipher(buf.data(), buf.size(), key);
    }
    if (progress) *progress = 0.7f;
    std::string fname = filename;
    if (fname.size() < 4 || fname.substr(fname.size() - 4) != ".txt") fname += ".txt";
    std::string outpath = ENC_TEXT_DIR + "/" + fname;
    writeBinaryFile(outpath, buf);
    if (progress) *progress = 1.0f;
    return outpath;
}

std::string encryptTextFile(const std::string& inputPath, const std::string& key,
                            CipherMethod method, std::atomic<float>* progress) {
    if (key.empty()) throw std::runtime_error("Key is empty.");
    if (progress) *progress = 0.2f;
    auto buf = readBinaryFile(inputPath);
    if (progress) *progress = 0.4f;
    if (method == CipherMethod::AES256) {
#ifdef _WIN32
        buf = aesEncryptBuffer(buf, key);
#else
        throw std::runtime_error("AES-256 not supported on this platform.");
#endif
    } else {
        xorCipher(buf.data(), buf.size(), key);
    }
    if (progress) *progress = 0.7f;
    std::string outpath = ENC_TEXT_DIR + "/" + fs::path(inputPath).filename().string();
    writeBinaryFile(outpath, buf);
    if (progress) *progress = 1.0f;
    return outpath;
}

std::string decryptTextFile(const std::string& encryptedPath,
                            const std::string& key,
                            std::string& savedPath,
                            CipherMethod method,
                            std::atomic<float>* progress) {
    if (key.empty()) throw std::runtime_error("Key is empty.");
    if (progress) *progress = 0.2f;
    auto buf = readBinaryFile(encryptedPath);
    if (progress) *progress = 0.4f;
    if (method == CipherMethod::AES256) {
#ifdef _WIN32
        buf = aesDecryptBuffer(buf, key);
#else
        throw std::runtime_error("AES-256 not supported on this platform.");
#endif
    } else {
        xorCipher(buf.data(), buf.size(), key);
    }
    if (progress) *progress = 0.7f;
    savedPath = DEC_TEXT_DIR + "/" + fs::path(encryptedPath).filename().string();
    writeBinaryFile(savedPath, buf);
    if (progress) *progress = 1.0f;
    return std::string(buf.begin(), buf.end());
}

std::string encryptImageFile(const std::string& inputPath, const std::string& key,
                             CipherMethod method, std::atomic<float>* progress) {
    if (key.empty()) throw std::runtime_error("Key is empty.");
    std::string ext = fs::path(inputPath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    if (ext != ".jpg" && ext != ".jpeg" && ext != ".png")
        throw std::runtime_error("Only .jpg, .jpeg, and .png files are supported.");
    if (progress) *progress = 0.2f;
    auto buf = readBinaryFile(inputPath);
    if (progress) *progress = 0.4f;
    if (method == CipherMethod::AES256) {
#ifdef _WIN32
        buf = aesEncryptBuffer(buf, key);
#else
        throw std::runtime_error("AES-256 not supported on this platform.");
#endif
    } else {
        xorCipher(buf.data(), buf.size(), key);
        scrambleBytes(buf.data(), buf.size(), key);
    }
    if (progress) *progress = 0.7f;
    std::string outpath = ENC_IMG_DIR + "/" + fs::path(inputPath).stem().string()
                        + ".enc" + fs::path(inputPath).extension().string();
    writeBinaryFile(outpath, buf);
    if (progress) *progress = 1.0f;
    return outpath;
}

std::string decryptImageFile(const std::string& encryptedPath, const std::string& key,
                             CipherMethod method, std::atomic<float>* progress) {
    if (key.empty()) throw std::runtime_error("Key is empty.");
    if (progress) *progress = 0.2f;
    auto buf = readBinaryFile(encryptedPath);
    if (progress) *progress = 0.4f;
    if (method == CipherMethod::AES256) {
#ifdef _WIN32
        buf = aesDecryptBuffer(buf, key);
#else
        throw std::runtime_error("AES-256 not supported on this platform.");
#endif
    } else {
        unscrambleBytes(buf.data(), buf.size(), key);
        xorCipher(buf.data(), buf.size(), key);
    }
    if (progress) *progress = 0.7f;
    std::string outname = fs::path(encryptedPath).filename().string();
    auto pos = outname.find(".enc");
    if (pos != std::string::npos) outname.erase(pos, 4);
    std::string outpath = DEC_IMG_DIR + "/" + outname;
    writeBinaryFile(outpath, buf);
    if (progress) *progress = 1.0f;
    return outpath;
}

std::vector<std::string> listEncryptedTextFiles() {
    std::vector<std::string> result;
    try {
        for (const auto& e : fs::directory_iterator(ENC_TEXT_DIR))
            if (e.is_regular_file()) result.push_back(e.path().filename().string());
    } catch (...) {}
    return result;
}

std::vector<std::string> listEncryptedImageFiles() {
    std::vector<std::string> result;
    try {
        for (const auto& e : fs::directory_iterator(ENC_IMG_DIR)) {
            std::string name = e.path().filename().string();
            if (e.is_regular_file() && name.find(".enc.") != std::string::npos)
                result.push_back(name);
        }
    } catch (...) {}
    return result;
}
