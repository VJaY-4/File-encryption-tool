/*
 * utils.cpp – Shared utilities for the encryption tool.
 */

#include "utils.h"

#include <iostream>
#include <string>
#include <stdexcept>
#include <filesystem>

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

using namespace std;
namespace fs = filesystem;

const string ENC_TEXT_DIR = "Encrypted Files/Text Files";
const string ENC_IMG_DIR  = "Encrypted Files/Images";
const string DEC_TEXT_DIR = "Decrypted Files/Text Files";
const string DEC_IMG_DIR  = "Decrypted Files/Images";
const string INPUT_DIR    = "Input Files";

// XOR each byte of buf with the key.
void xorCipher(char* buf, size_t len, const string& key) {
    for (size_t i = 0; i < len; ++i)
        buf[i] ^= key[i % key.size()];
}

// Read a line from stdin with masked input (each character shown as '*').
static string readMaskedLine() {
    string result;
#ifdef _WIN32
    int ch;
    while ((ch = _getch()) != '\r') {   // Enter key
        if (ch == '\b' || ch == 127) {   // Backspace
            if (!result.empty()) {
                result.pop_back();
                cout << "\b \b";
            }
        } else if (ch >= 32) {           // Printable character
            result += static_cast<char>(ch);
            cout << '*';
        }
    }
    cout << '\n';
#else
    // Disable terminal echo on Unix
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    char ch;
    while (read(STDIN_FILENO, &ch, 1) == 1 && ch != '\n') {
        if (ch == 127 || ch == '\b') {
            if (!result.empty()) {
                result.pop_back();
                cout << "\b \b";
            }
        } else if (ch >= 32) {
            result += ch;
            cout << '*';
        }
    }
    cout << '\n';

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif
    return result;
}

// Prompt the user for a key with masked input and validate minimum length.
string readKey(const char* prompt) {
    cout << prompt;
    string key = readMaskedLine();
    if (key.empty()) throw invalid_argument("Key cannot be empty.");
    if (key.size() < 4) {
        cout << "[WARNING] Key is very short (" << key.size()
             << " chars). Short keys are easy to break.\n"
             << "Continue anyway? (y/n): ";
        char c;
        cin.get(c);
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if (c != 'y' && c != 'Y')
            throw invalid_argument("Encryption cancelled. Use a longer key (4+ characters).");
    }
    return key;
}

// Strip trailing whitespace, carriage returns, and newlines.
void trimTrailing(string& s) {
    while (!s.empty() && (s.back() == ' ' || s.back() == '\r' || s.back() == '\n'))
        s.pop_back();
}

// Ask user to confirm overwriting an existing file.
bool confirmOverwrite(const string& filepath) {
    if (!fs::exists(filepath)) return true;
    cout << "[WARNING] File already exists: " << filepath << "\n"
         << "Overwrite? (y/n): ";
    char c;
    cin.get(c);
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return (c == 'y' || c == 'Y');
}
