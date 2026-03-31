/*
 * crypto_core.cpp – Non-interactive encryption/decryption logic for GUI use.
 * Wraps XOR cipher + image byte-scrambling into clean functions.
 */

#include "crypto_core.h"

#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <algorithm>

namespace fs = std::filesystem;

// ── Folder paths ─────────────────────────────────────────────

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

// ── Shared utilities ─────────────────────────────────────────

void xorCipher(char* buf, size_t len, const std::string& key) {
    if (key.empty()) return;
    for (size_t i = 0; i < len; ++i)
        buf[i] ^= key[i % key.size()];
}

void trimTrailing(std::string& s) {
    while (!s.empty() && (s.back() == ' ' || s.back() == '\r' || s.back() == '\n'))
        s.pop_back();
}

// ── Image scrambling helpers ─────────────────────────────────

static unsigned int keyToSeed(const std::string& key) {
    unsigned int seed = 5381;
    for (char c : key)
        seed = seed * 33 + static_cast<unsigned char>(c);
    return seed;
}

static unsigned int lcgNext(unsigned int& state) {
    state = state * 1103515245 + 12345;
    return (state >> 16) & 0x7FFF;
}

static void scrambleBytes(char* buf, size_t len, const std::string& key) {
    if (len <= 1) return;
    unsigned int state = keyToSeed(key);
    size_t numSwaps = std::min(len, static_cast<size_t>(300000));
    for (size_t s = 0; s < numSwaps; ++s) {
        size_t i = lcgNext(state) % len;
        size_t j = lcgNext(state) % len;
        std::swap(buf[i], buf[j]);
    }
}

static void unscrambleBytes(char* buf, size_t len, const std::string& key) {
    if (len <= 1) return;
    unsigned int state = keyToSeed(key);
    size_t numSwaps = std::min(len, static_cast<size_t>(300000));

    std::vector<std::pair<size_t, size_t>> swaps(numSwaps);
    for (size_t s = 0; s < numSwaps; ++s) {
        swaps[s].first  = lcgNext(state) % len;
        swaps[s].second = lcgNext(state) % len;
    }
    for (size_t s = numSwaps; s > 0; --s)
        std::swap(buf[swaps[s - 1].first], buf[swaps[s - 1].second]);
}

// ── File I/O helpers ─────────────────────────────────────────

static std::vector<char> readBinaryFile(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) throw std::runtime_error("Cannot open: " + path);
    in.seekg(0, std::ios::end);
    size_t sz = static_cast<size_t>(in.tellg());
    in.seekg(0, std::ios::beg);
    if (sz == 0) throw std::runtime_error("File is empty: " + path);
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

// ── Text encrypt/decrypt ─────────────────────────────────────

std::string encryptTextFromString(const std::string& content,
                                  const std::string& filename,
                                  const std::string& key) {
    if (content.empty()) throw std::runtime_error("Content is empty.");
    if (key.empty()) throw std::runtime_error("Key is empty.");

    std::vector<char> buf(content.begin(), content.end());
    xorCipher(buf.data(), buf.size(), key);

    std::string fname = filename;
    if (fname.size() < 4 || fname.substr(fname.size() - 4) != ".txt")
        fname += ".txt";

    std::string outpath = ENC_TEXT_DIR + "/" + fname;
    writeBinaryFile(outpath, buf);
    return outpath;
}

std::string encryptTextFile(const std::string& inputPath,
                            const std::string& key) {
    if (key.empty()) throw std::runtime_error("Key is empty.");
    std::vector<char> buf = readBinaryFile(inputPath);
    xorCipher(buf.data(), buf.size(), key);

    std::string outname = fs::path(inputPath).filename().string();
    std::string outpath = ENC_TEXT_DIR + "/" + outname;
    writeBinaryFile(outpath, buf);
    return outpath;
}

std::string decryptTextFile(const std::string& encryptedPath,
                            const std::string& key,
                            std::string& savedPath) {
    if (key.empty()) throw std::runtime_error("Key is empty.");
    std::vector<char> buf = readBinaryFile(encryptedPath);
    xorCipher(buf.data(), buf.size(), key);

    std::string filename = fs::path(encryptedPath).filename().string();
    savedPath = DEC_TEXT_DIR + "/" + filename;
    writeBinaryFile(savedPath, buf);

    return std::string(buf.begin(), buf.end());
}

// ── Image encrypt/decrypt ────────────────────────────────────

std::string encryptImageFile(const std::string& inputPath,
                             const std::string& key) {
    if (key.empty()) throw std::runtime_error("Key is empty.");

    std::string ext = fs::path(inputPath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    if (ext != ".jpg" && ext != ".jpeg" && ext != ".png")
        throw std::runtime_error("Only .jpg, .jpeg, and .png files are supported.");

    std::vector<char> buf = readBinaryFile(inputPath);
    xorCipher(buf.data(), buf.size(), key);
    scrambleBytes(buf.data(), buf.size(), key);

    std::string stem = fs::path(inputPath).stem().string();
    std::string origExt = fs::path(inputPath).extension().string();
    std::string outpath = ENC_IMG_DIR + "/" + stem + ".enc" + origExt;
    writeBinaryFile(outpath, buf);
    return outpath;
}

std::string decryptImageFile(const std::string& encryptedPath,
                             const std::string& key) {
    if (key.empty()) throw std::runtime_error("Key is empty.");

    std::vector<char> buf = readBinaryFile(encryptedPath);
    unscrambleBytes(buf.data(), buf.size(), key);
    xorCipher(buf.data(), buf.size(), key);

    std::string outname = fs::path(encryptedPath).filename().string();
    size_t encPos = outname.find(".enc");
    if (encPos != std::string::npos)
        outname.erase(encPos, 4);

    std::string outpath = DEC_IMG_DIR + "/" + outname;
    writeBinaryFile(outpath, buf);
    return outpath;
}

// ── Listing helpers ──────────────────────────────────────────

std::vector<std::string> listEncryptedTextFiles() {
    std::vector<std::string> result;
    try {
        for (const auto& e : fs::directory_iterator(ENC_TEXT_DIR))
            if (e.is_regular_file())
                result.push_back(e.path().filename().string());
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
