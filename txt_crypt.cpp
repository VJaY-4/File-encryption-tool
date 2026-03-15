/*
 * txt_crypt.cpp – Text file encryption / decryption using XOR cipher.
 */

#include "txt_crypt.h"
#include "utils.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <filesystem>
#include <limits>

using namespace std;
namespace fs = filesystem;

// Show all files inside the encrypted text files folder.
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

    trimTrailing(filepath);

    if (!fs::exists(filepath))
        throw runtime_error("File not found: " + filepath);

    return filepath;
}

// Encrypt: asks for a filename, a message, and a key, then saves the encrypted file.
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

        string filename;
        cout << "\nEnter filename (without .txt): ";
        getline(cin, filename);
        if (filename.empty()) throw invalid_argument("Filename cannot be empty.");
        for (char c : filename)
            if (c == '/' || c == '\\' || c == ':' || c == '*' ||
                c == '?' || c == '"' || c == '<' || c == '>' || c == '|')
                throw invalid_argument("Filename contains invalid characters.");
        filename += ".txt";

        vector<char> buf;

        cout << "\nType your message (Ctrl+Z Enter on Windows / Ctrl+D on Unix to finish):\n";

        string line;
        while (getline(cin, line)) {
            buf.insert(buf.end(), line.begin(), line.end());
            buf.push_back('\n');
        }
        cin.clear();
        if (buf.empty()) throw invalid_argument("No content entered.");

        string key = readKey("Enter encryption key: ");
        xorCipher(buf.data(), buf.size(), key);

        string path = ENC_TEXT_DIR + "/" + filename;
        if (!confirmOverwrite(path))
            throw runtime_error("Operation cancelled by user.");

        ofstream out(path, ios::binary);
        if (!out) throw runtime_error("Cannot create: " + path);
        out.write(buf.data(), static_cast<streamsize>(buf.size()));
        if (out.fail()) throw runtime_error("Write error: " + path);

        cout << "\n[SUCCESS] Saved to: " << path << "\n";

    } else if (srcChoice == 2) {
        // --- Select an existing file and encrypt it ---

        string filepath = getTextFilePath();

        ifstream in(filepath, ios::binary);
        if (!in) throw runtime_error("Cannot open: " + filepath);

        in.seekg(0, ios::end);
        size_t size = static_cast<size_t>(in.tellg());
        in.seekg(0, ios::beg);
        if (size == 0) throw runtime_error("File is empty.");

        vector<char> buf(size);
        in.read(buf.data(), static_cast<streamsize>(size));
        in.close();

        string key = readKey("Enter encryption key: ");
        xorCipher(buf.data(), buf.size(), key);

        string outname = fs::path(filepath).filename().string();
        string path = ENC_TEXT_DIR + "/" + outname;
        if (!confirmOverwrite(path))
            throw runtime_error("Operation cancelled by user.");

        ofstream out(path, ios::binary);
        if (!out) throw runtime_error("Cannot create: " + path);
        out.write(buf.data(), static_cast<streamsize>(buf.size()));
        if (out.fail()) throw runtime_error("Write error: " + path);

        cout << "\n[SUCCESS] Saved to: " << path << "\n";

    } else {
        throw invalid_argument("Invalid choice. Enter 1 or 2.");
    }
}

// Decrypt: reads an encrypted file, unscrambles it with the key, and shows the result.
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
            trimTrailing(path);
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

    ifstream in(path, ios::binary);
    if (!in) throw runtime_error("File not found: " + path);

    in.seekg(0, ios::end);
    size_t size = static_cast<size_t>(in.tellg());
    in.seekg(0, ios::beg);
    if (size == 0) throw runtime_error("File is empty: " + path);

    vector<char> buf(size + 1);
    in.read(buf.data(), static_cast<streamsize>(size));
    size_t n = static_cast<size_t>(in.gcount());
    buf[n] = '\0';

    xorCipher(buf.data(), n, key);

    cout << "\n+---------- Decrypted Content ----------+\n";
    cout << buf.data();
    if (buf[n - 1] != '\n') cout << '\n';
    cout << "+---------------------------------------+\n";

    string outpath = DEC_TEXT_DIR + "/" + filename;
    if (!confirmOverwrite(outpath))
        throw runtime_error("Operation cancelled by user.");

    ofstream out(outpath, ios::binary);
    if (!out) throw runtime_error("Cannot create: " + outpath);
    out.write(buf.data(), static_cast<streamsize>(n));
    if (out.fail()) throw runtime_error("Write error: " + outpath);

    cout << "[SUCCESS] Decrypted file saved to: " << outpath << "\n";
}
