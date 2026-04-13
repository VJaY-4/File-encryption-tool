
#include "txt_crypt.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <filesystem>
#include <limits>
using namespace std; namespace fs = filesystem;

static void listEncryptedFiles() {
    cout << "\nFiles in '" << ENC_TEXT_DIR << "':\n";
    bool found = false;
    try {
        for (const auto& e : fs::directory_iterator(ENC_TEXT_DIR))
            if (e.is_regular_file()) { cout << "  - " << e.path().filename().string() << "\n"; found = true; }
    } catch (const fs::filesystem_error&) {}
    if (!found) cout << "  (none)\n";
}

static string getTextFilePath() {
    cout << "\n--- File Location ---\n  1. Enter full path\n  2. Filename in Input Files\nChoice: ";
    int choice;
    if (!(cin >> choice)) {
        cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
        throw invalid_argument("Invalid choice.");
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    string filepath;
    if (choice == 1) {
        cout << "Enter full path: "; getline(cin, filepath);
    } else if (choice == 2) {
        cout << "Enter filename (e.g., test1.txt): "; getline(cin, filepath);
        if (filepath.size() < 4 || filepath.substr(filepath.size() - 4) != ".txt") filepath += ".txt";
        filepath = INPUT_DIR + "/" + filepath;
    } else { throw invalid_argument("Invalid choice. Enter 1 or 2."); }
    if (filepath.empty()) throw invalid_argument("Path cannot be empty.");
    trimTrailing(filepath);
    if (!fs::exists(filepath)) throw runtime_error("File not found: " + filepath);
    return filepath;
}

static vector<char> readFile(const string& path) {
    ifstream in(path, ios::binary | ios::ate);
    if (!in) throw runtime_error("Cannot open: " + path);
    auto sz = static_cast<size_t>(in.tellg());
    if (sz == 0) throw runtime_error("File is empty.");
    in.seekg(0);
    vector<char> buf(sz);
    in.read(buf.data(), static_cast<streamsize>(sz));
    return buf;
}

static void writeFile(const string& path, const vector<char>& buf) {
    ofstream out(path, ios::binary);
    if (!out) throw runtime_error("Cannot create: " + path);
    out.write(buf.data(), static_cast<streamsize>(buf.size()));
    if (out.fail()) throw runtime_error("Write error: " + path);
}

void encryptFile() {
    cout << "\n--- Source ---\n  1. Create new file (type content)\n  2. Select existing file\nChoice: ";
    int srcChoice;
    if (!(cin >> srcChoice)) {
        cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
        throw invalid_argument("Invalid choice. Enter 1 or 2.");
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (srcChoice == 1) {
        string filename;
        cout << "\nEnter filename (without .txt): "; getline(cin, filename);
        if (filename.empty()) throw invalid_argument("Filename cannot be empty.");
        for (char c : filename)
            if (c == '/' || c == '\\' || c == ':' || c == '*' ||
                c == '?' || c == '"' || c == '<' || c == '>' || c == '|')
                throw invalid_argument("Filename contains invalid characters.");
        filename += ".txt";
        vector<char> buf;
        cout << "\nType your message (Ctrl+Z Enter on Windows / Ctrl+D on Unix to finish):\n";
        string line;
        while (getline(cin, line)) { buf.insert(buf.end(), line.begin(), line.end()); buf.push_back('\n'); }
        cin.clear();
        if (buf.empty()) throw invalid_argument("No content entered.");
        string key = readKey("Enter encryption key: ");
        xorCipher(buf.data(), buf.size(), key);
        string path = ENC_TEXT_DIR + "/" + filename;
        if (!confirmOverwrite(path)) throw runtime_error("Operation cancelled by user.");
        writeFile(path, buf);
        cout << "\n[SUCCESS] Saved to: " << path << "\n";
    } else if (srcChoice == 2) {
        string filepath = getTextFilePath();
        auto buf = readFile(filepath);
        string key = readKey("Enter encryption key: ");
        xorCipher(buf.data(), buf.size(), key);
        string path = ENC_TEXT_DIR + "/" + fs::path(filepath).filename().string();
        if (!confirmOverwrite(path)) throw runtime_error("Operation cancelled by user.");
        writeFile(path, buf);
        cout << "\n[SUCCESS] Saved to: " << path << "\n";
    } else {
        throw invalid_argument("Invalid choice. Enter 1 or 2.");
    }
}

void decryptFile() {
    listEncryptedFiles();
    cout << "\n--- File Location ---\n  1. Enter full path\n  2. Filename from Encrypted Files/Text Files\nChoice: ";
    int locChoice;
    if (!(cin >> locChoice)) {
        cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
        throw invalid_argument("Invalid choice. Enter 1 or 2.");
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    string path, filename;
    if (locChoice == 1) {
        cout << "Enter full path: "; getline(cin, path);
        if (path.empty()) throw invalid_argument("Path cannot be empty.");
        trimTrailing(path);
        filename = fs::path(path).filename().string();
    } else if (locChoice == 2) {
        cout << "Enter filename (without .txt): "; getline(cin, filename);
        if (filename.empty()) throw invalid_argument("Filename cannot be empty.");
        filename += ".txt";
        path = ENC_TEXT_DIR + "/" + filename;
    } else { throw invalid_argument("Invalid choice. Enter 1 or 2."); }

    string key = readKey("Enter decryption key: ");
    auto buf = readFile(path);
    size_t n = buf.size();
    buf.push_back('\0');
    xorCipher(buf.data(), n, key);

    cout << "\n+---------- Decrypted Content ----------+\n";
    cout << buf.data();
    if (buf[n - 1] != '\n') cout << '\n';
    cout << "+---------------------------------------+\n";

    string outpath = DEC_TEXT_DIR + "/" + filename;
    if (!confirmOverwrite(outpath)) throw runtime_error("Operation cancelled by user.");
    buf.pop_back();
    writeFile(outpath, buf);
    cout << "[SUCCESS] Decrypted file saved to: " << outpath << "\n";
}
