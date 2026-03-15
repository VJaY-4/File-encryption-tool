/*
 * txt_crypt.cpp – Text file encryption / decryption using XOR cipher.
 */

#include "txt_crypt.h"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <filesystem>
#include <cstring>
#include <limits>

using namespace std;
namespace fs = filesystem;

const string ENC_TEXT_DIR = "Encrypted Files/Text Files";
const string ENC_IMG_DIR  = "Encrypted Files/Images";
const string DEC_TEXT_DIR = "Decrypted Files/Text Files";
const string DEC_IMG_DIR  = "Decrypted Files/Images";
const string INPUT_DIR    = "Input Files";

// Scrambles (or unscrambles) data by XOR-ing each byte with the key (CO1: pointer arithmetic).
// Running it twice with the same key gives back the original text.
static void xorCipher(char* buf, size_t len, const string& key) {
    for (size_t i = 0; i < len; ++i)
        *(buf + i) ^= key[i % key.size()];
}

// Ask the user to type a key and give it back as a string.
// The key must not be empty, otherwise the encryption would do nothing.
static string readKey(const char* prompt) {
    cout << prompt;
    string key;
    getline(cin, key);
    if (key.empty()) throw invalid_argument("Key cannot be empty.");
    return key;
}

// Show all files inside the encrypted files folder.
// This lets the user see what files they can decrypt.
static void listEncryptedFiles() {
    cout << "\nFiles in '" << ENC_TEXT_DIR << "':\n";
    bool found = false;
    try {
        for (const auto& e : fs::directory_iterator(ENC_TEXT_DIR))
            if (e.is_regular_file()) {
                cout << "  - " << e.path().filename().string() << "\n";
                found = true;
            }
    } catch (const fs::filesystem_error&) {}
    if (!found) cout << "  (none)\n";
}

// Ask the user how they want to locate the text file.
// Method 1: enter the full path     Method 2: filename in the project folder
static string getTextFilePath() {
    cout << "\n--- File Location ---\n"
         << "  1. Enter full path to file\n"
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
            cout << "Enter full path to file: ";
            getline(cin, filepath);
            break;
        case 2:
            cout << "Enter filename (e.g., test1.txt): ";
            getline(cin, filepath);
            if (filepath.size() < 4 || filepath.substr(filepath.size() - 4) != ".txt")
                filepath += ".txt";
            filepath = INPUT_DIR + "/" + filepath;
            break;
        default:
            throw invalid_argument("Invalid choice. Enter 1 or 2.");
    }

    if (filepath.empty())
        throw invalid_argument("Path cannot be empty.");

    // Strip trailing whitespace that may sneak in from input.
    while (!filepath.empty() && (filepath.back() == ' ' || filepath.back() == '\r' || filepath.back() == '\n'))
        filepath.pop_back();

    if (!fs::exists(filepath))
        throw runtime_error("File not found: " + filepath);

    return filepath;
}

// Encrypt: asks for a filename, a message, and a key, then saves the encrypted file.
// The encrypted output goes into the encrypted files folder.
void encryptFile() {
    cout << "\n--- Source ---\n"
         << "  1. Create a new file (type content)\n"
         << "  2. Select an existing file\n"
         << "Choice: ";

    int srcChoice;
    if (!(cin >> srcChoice)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        throw invalid_argument("Invalid choice. Enter 1 or 2.");
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (srcChoice == 1) {
        // --- Create a new file: type content and encrypt it ---

        // Ask the user for a filename and make sure it is valid.
        // Block any characters that are not allowed in file names.
        string filename;
        cout << "\nEnter filename (without .txt): ";
        getline(cin, filename);
        if (filename.empty()) throw invalid_argument("Filename cannot be empty.");
        for (char c : filename)
            if (c == '/' || c == '\\' || c == ':' || c == '*' ||
                c == '?' || c == '"' || c == '<' || c == '>' || c == '|')
                throw invalid_argument("Filename contains invalid characters.");
        filename += ".txt";

        // Create a buffer on the heap to hold the user's typed message (CO1: new[]/delete[]).
        // The buffer grows bigger automatically if the message is long.
        size_t cap = 1024, len = 0;
        char* buf = new char[cap];

        cout << "\nType your message (Ctrl+Z Enter on Windows / Ctrl+D on Unix to finish):\n";

        try {
            // Keep reading lines until the user signals they are done (Ctrl+Z / Ctrl+D).
            // Each line is added to the buffer, making it bigger if it runs out of room.
            string line;
            while (getline(cin, line)) {
                size_t needed = len + line.size() + 2;
                if (needed > cap) {
                    while (cap < needed) cap *= 2;
                    char* tmp = new char[cap];
                    memcpy(tmp, buf, len);
                    delete[] buf;
                    buf = tmp;
                }
                memcpy(buf + len, line.c_str(), line.size());
                len += line.size();
                buf[len++] = '\n';
            }
            cin.clear();
            if (len == 0) throw invalid_argument("No content entered.");

            // Ask for the key and scramble the message using XOR.
            string key = readKey("Enter encryption key: ");
            xorCipher(buf, len, key);

            // Save the encrypted data to a file in the output folder.
            string path = ENC_TEXT_DIR + "/" + filename;
            ofstream out(path, ios::binary);
            if (!out) throw runtime_error("Cannot create: " + path);
            out.write(buf, static_cast<streamsize>(len));
            if (out.fail()) throw runtime_error("Write error: " + path);

            delete[] buf;
            cout << "\n[SUCCESS] Saved to: " << path << "\n";
        } catch (...) {
            delete[] buf;
            throw;
        }

    } else if (srcChoice == 2) {
        // --- Select an existing file and encrypt it ---

        string filepath = getTextFilePath();

        // Read the entire file into a buffer.
        ifstream in(filepath, ios::binary);
        if (!in) throw runtime_error("Cannot open: " + filepath);

        in.seekg(0, ios::end);
        size_t size = static_cast<size_t>(in.tellg());
        in.seekg(0, ios::beg);
        if (size == 0) throw runtime_error("File is empty.");

        char* buf = new char[size];
        try {
            in.read(buf, static_cast<streamsize>(size));
            in.close();

            string key = readKey("Enter encryption key: ");
            xorCipher(buf, size, key);

            // Use the original filename for the encrypted output.
            string outname = fs::path(filepath).filename().string();
            string path = ENC_TEXT_DIR + "/" + outname;
            ofstream out(path, ios::binary);
            if (!out) throw runtime_error("Cannot create: " + path);
            out.write(buf, static_cast<streamsize>(size));
            if (out.fail()) throw runtime_error("Write error: " + path);

            delete[] buf;
            cout << "\n[SUCCESS] Saved to: " << path << "\n";
        } catch (...) {
            delete[] buf;
            throw;
        }

    } else {
        throw invalid_argument("Invalid choice. Enter 1 or 2.");
    }
}

// Decrypt: reads an encrypted file, unscrambles it with the key, and shows the result.
// Shows the list of files first so the user knows what is available.
void decryptFile() {
    listEncryptedFiles();

    cout << "\n--- File Location ---\n"
         << "  1. Enter full path to encrypted file\n"
         << "  2. Enter filename (from Encrypted Files/Text Files)\n"
         << "Choice: ";

    int locChoice;
    if (!(cin >> locChoice)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        throw invalid_argument("Invalid choice. Enter 1 or 2.");
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    string path;
    string filename;
    switch (locChoice) {
        case 1:
            cout << "Enter full path to encrypted file: ";
            getline(cin, path);
            if (path.empty()) throw invalid_argument("Path cannot be empty.");
            while (!path.empty() && (path.back() == ' ' || path.back() == '\r' || path.back() == '\n'))
                path.pop_back();
            filename = fs::path(path).filename().string();
            break;
        case 2:
            cout << "Enter filename to decrypt (without .txt): ";
            getline(cin, filename);
            if (filename.empty()) throw invalid_argument("Filename cannot be empty.");
            filename += ".txt";
            path = ENC_TEXT_DIR + "/" + filename;
            break;
        default:
            throw invalid_argument("Invalid choice. Enter 1 or 2.");
    }

    string key = readKey("Enter decryption key: ");

    // Open the encrypted file in binary mode and find out how big it is.
    // Binary mode makes sure the bytes are read exactly as they were saved.
    ifstream in(path, ios::binary);
    if (!in) throw runtime_error("File not found: " + path);

    in.seekg(0, ios::end);
    size_t size = static_cast<size_t>(in.tellg());
    in.seekg(0, ios::beg);
    if (size == 0) throw runtime_error("File is empty: " + path);

    // Create a buffer big enough to hold the whole file (CO1: new[]/delete[]).
    // One extra byte is added for the null terminator at the end.
    char* buf = new char[size + 1];
    try {
        in.read(buf, static_cast<streamsize>(size));
        size_t n = static_cast<size_t>(in.gcount());
        buf[n] = '\0';

        // Unscramble the data using the same XOR function and key.
        // This turns the encrypted bytes back into readable text.
        xorCipher(buf, n, key);

        cout << "\n+---------- Decrypted Content ----------+\n";
        cout << buf;
        if (buf[n - 1] != '\n') cout << '\n';
        cout << "+---------------------------------------+\n";

        // Save decrypted text to the Decrypted Files folder.
        string outpath = DEC_TEXT_DIR + "/" + filename;
        ofstream out(outpath, ios::binary);
        if (!out) throw runtime_error("Cannot create: " + outpath);
        out.write(buf, static_cast<streamsize>(n));
        if (out.fail()) throw runtime_error("Write error: " + outpath);

        cout << "[SUCCESS] Decrypted file saved to: " << outpath << "\n";

        delete[] buf;
    } catch (...) {
        delete[] buf;
        throw;
    }
}
