/*
 * img_crypt.cpp – Image file encryption / decryption.
 * Supports .jpg, .jpeg, and .png files.
 * Uses XOR cipher + byte-position scrambling for double-layer encryption.
 */

#include "img_crypt.h"
#include "utils.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <filesystem>
#include <algorithm>
#include <limits>

using namespace std;
namespace fs = filesystem;

// ── helpers ──────────────────────────────────────────────────

// Derive a numeric seed from the key string.
static unsigned int keyToSeed(const string& key) {
    unsigned int seed = 5381;
    for (char c : key)
        seed = seed * 33 + static_cast<unsigned char>(c);
    return seed;
}

// Simple LCG pseudo-random number generator.
static unsigned int lcgNext(unsigned int& state) {
    state = state * 1103515245 + 12345;
    return (state >> 16) & 0x7FFF;
}

// Scramble byte positions using random swaps (forward direction).
static void scrambleBytes(char* buf, size_t len, const string& key) {
    if (len <= 1) return;
    unsigned int state = keyToSeed(key);
    size_t numSwaps = min(len, static_cast<size_t>(300000));
    for (size_t s = 0; s < numSwaps; ++s) {
        size_t i = lcgNext(state) % len;
        size_t j = lcgNext(state) % len;
        swap(buf[i], buf[j]);
    }
}

// Unscramble byte positions by replaying the same swaps in reverse order.
static void unscrambleBytes(char* buf, size_t len, const string& key) {
    if (len <= 1) return;
    unsigned int state = keyToSeed(key);
    size_t numSwaps = min(len, static_cast<size_t>(300000));

    vector<pair<size_t, size_t>> swaps(numSwaps);
    for (size_t s = 0; s < numSwaps; ++s) {
        swaps[s].first  = lcgNext(state) % len;
        swaps[s].second = lcgNext(state) % len;
    }
    for (size_t s = numSwaps; s > 0; --s)
        swap(buf[swaps[s - 1].first], buf[swaps[s - 1].second]);
}

// Ask the user how they want to locate the image file.
static string getImagePath() {
    cout << "\n--- Image Location ---\n"
         << "  1. Enter full path to image\n"
         << "  2. Enter filename (must be in Input Files folder)\n"
         << "Choice: ";

    int choice;
    if (!(cin >> choice)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        throw invalid_argument("Invalid choice.");
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    string filepath;
    switch (choice) {
        case 1:
            cout << "Enter full path to image: ";
            getline(cin, filepath);
            break;
        case 2:
            cout << "Enter image filename (e.g., photo.png): ";
            getline(cin, filepath);
            filepath = INPUT_DIR + "/" + filepath;
            break;
        default:
            throw invalid_argument("Invalid choice. Enter 1 or 2.");
    }

    if (filepath.empty())
        throw invalid_argument("Path cannot be empty.");

    trimTrailing(filepath);

    // Make sure the file has a supported image extension.
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos == string::npos)
        throw invalid_argument("File must have a .jpg or .png extension.");

    string ext = filepath.substr(dotPos);
    transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    if (ext != ".jpg" && ext != ".jpeg" && ext != ".png")
        throw invalid_argument("Only .jpg, .jpeg, and .png files are supported.");

    if (!fs::exists(filepath))
        throw runtime_error("File not found: " + filepath);

    return filepath;
}

// ── public API ───────────────────────────────────────────────

void encryptImage() {
    string filepath = getImagePath();
    string key = readKey("Enter encryption key: ");

    ifstream in(filepath, ios::binary);
    if (!in) throw runtime_error("Cannot open: " + filepath);

    in.seekg(0, ios::end);
    size_t size = static_cast<size_t>(in.tellg());
    in.seekg(0, ios::beg);
    if (size == 0) throw runtime_error("File is empty.");

    vector<char> buf(size);
    in.read(buf.data(), static_cast<streamsize>(size));
    in.close();

    // Step 1: XOR encrypt every byte.
    xorCipher(buf.data(), buf.size(), key);
    // Step 2: Scramble byte positions for extra security.
    scrambleBytes(buf.data(), buf.size(), key);

    string stem = fs::path(filepath).stem().string();
    string ext  = fs::path(filepath).extension().string();
    string outpath = ENC_IMG_DIR + "/" + stem + ".enc" + ext;

    if (!confirmOverwrite(outpath))
        throw runtime_error("Operation cancelled by user.");

    ofstream out(outpath, ios::binary);
    if (!out) throw runtime_error("Cannot create: " + outpath);
    out.write(buf.data(), static_cast<streamsize>(buf.size()));
    if (out.fail()) throw runtime_error("Write error: " + outpath);

    cout << "\n[SUCCESS] Encrypted image saved to: " << outpath << "\n";
}

void decryptImage() {
    // Show encrypted image files so the user knows what is available.
    cout << "\nEncrypted images in '" << ENC_IMG_DIR << "':\n";
    bool found = false;
    try {
        for (const auto& e : fs::directory_iterator(ENC_IMG_DIR)) {
            string name = e.path().filename().string();
            if (e.is_regular_file() && name.find(".enc.") != string::npos) {
                cout << "  - " << name << "\n";
                found = true;
            }
        }
    } catch (const fs::filesystem_error&) {}
    if (!found) cout << "  (none)\n";

    cout << "\nEnter encrypted image filename (e.g., photo.enc.png): ";
    string filename;
    getline(cin, filename);
    trimTrailing(filename);
    if (filename.empty()) throw invalid_argument("Filename cannot be empty.");

    string key = readKey("Enter decryption key: ");

    string path = ENC_IMG_DIR + "/" + filename;
    ifstream in(path, ios::binary);
    if (!in) throw runtime_error("File not found: " + path);

    in.seekg(0, ios::end);
    size_t size = static_cast<size_t>(in.tellg());
    in.seekg(0, ios::beg);
    if (size == 0) throw runtime_error("File is empty.");

    vector<char> buf(size);
    in.read(buf.data(), static_cast<streamsize>(size));
    in.close();

    // Step 1: Unscramble byte positions (reverse of encrypt step 2).
    unscrambleBytes(buf.data(), buf.size(), key);
    // Step 2: XOR decrypt (reverse of encrypt step 1).
    xorCipher(buf.data(), buf.size(), key);

    // Remove ".enc" from the filename to reconstruct the original name.
    string outname = filename;
    size_t encPos = outname.find(".enc");
    if (encPos != string::npos)
        outname.erase(encPos, 4);
    string outpath = DEC_IMG_DIR + "/" + outname;

    if (!confirmOverwrite(outpath))
        throw runtime_error("Operation cancelled by user.");

    ofstream out(outpath, ios::binary);
    if (!out) throw runtime_error("Cannot create: " + outpath);
    out.write(buf.data(), static_cast<streamsize>(buf.size()));
    if (out.fail()) throw runtime_error("Write error: " + outpath);

    cout << "\n[SUCCESS] Decrypted image saved to: " << outpath << "\n";
}
