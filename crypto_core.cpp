#include "crypto_core.h"

#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <algorithm>

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

std::string encryptTextFromString(const std::string& content,
                                  const std::string& filename,
                                  const std::string& key) {
    if (content.empty()) throw std::runtime_error("Content is empty.");
    if (key.empty())     throw std::runtime_error("Key is empty.");
    std::vector<char> buf(content.begin(), content.end());
    xorCipher(buf.data(), buf.size(), key);
    std::string fname = filename;
    if (fname.size() < 4 || fname.substr(fname.size() - 4) != ".txt") fname += ".txt";
    std::string outpath = ENC_TEXT_DIR + "/" + fname;
    writeBinaryFile(outpath, buf);
    return outpath;
}

std::string encryptTextFile(const std::string& inputPath, const std::string& key) {
    if (key.empty()) throw std::runtime_error("Key is empty.");
    auto buf = readBinaryFile(inputPath);
    xorCipher(buf.data(), buf.size(), key);
    std::string outpath = ENC_TEXT_DIR + "/" + fs::path(inputPath).filename().string();
    writeBinaryFile(outpath, buf);
    return outpath;
}

std::string decryptTextFile(const std::string& encryptedPath,
                            const std::string& key,
                            std::string& savedPath) {
    if (key.empty()) throw std::runtime_error("Key is empty.");
    auto buf = readBinaryFile(encryptedPath);
    xorCipher(buf.data(), buf.size(), key);
    savedPath = DEC_TEXT_DIR + "/" + fs::path(encryptedPath).filename().string();
    writeBinaryFile(savedPath, buf);
    return std::string(buf.begin(), buf.end());
}

std::string encryptImageFile(const std::string& inputPath, const std::string& key) {
    if (key.empty()) throw std::runtime_error("Key is empty.");
    std::string ext = fs::path(inputPath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    if (ext != ".jpg" && ext != ".jpeg" && ext != ".png")
        throw std::runtime_error("Only .jpg, .jpeg, and .png files are supported.");
    auto buf = readBinaryFile(inputPath);
    xorCipher(buf.data(), buf.size(), key);
    scrambleBytes(buf.data(), buf.size(), key);
    std::string outpath = ENC_IMG_DIR + "/" + fs::path(inputPath).stem().string()
                        + ".enc" + fs::path(inputPath).extension().string();
    writeBinaryFile(outpath, buf);
    return outpath;
}

std::string decryptImageFile(const std::string& encryptedPath, const std::string& key) {
    if (key.empty()) throw std::runtime_error("Key is empty.");
    auto buf = readBinaryFile(encryptedPath);
    unscrambleBytes(buf.data(), buf.size(), key);
    xorCipher(buf.data(), buf.size(), key);
    std::string outname = fs::path(encryptedPath).filename().string();
    auto pos = outname.find(".enc");
    if (pos != std::string::npos) outname.erase(pos, 4);
    std::string outpath = DEC_IMG_DIR + "/" + outname;
    writeBinaryFile(outpath, buf);
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
