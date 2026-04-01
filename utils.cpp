#include "utils.h"

#include <iostream>
#include <filesystem>
#include <limits>
#include <stdexcept>

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

namespace fs = std::filesystem;
using std::string, std::cout, std::cin;

static string readMaskedLine() {
    string result;
#ifdef _WIN32
    int ch;
    while ((ch = _getch()) != '\r') {
        if (ch == '\b' || ch == 127) {
            if (!result.empty()) { result.pop_back(); cout << "\b \b"; }
        } else if (ch >= 32) { result += static_cast<char>(ch); cout << '*'; }
    }
    cout << '\n';
#else
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    char ch;
    while (read(STDIN_FILENO, &ch, 1) == 1 && ch != '\n') {
        if (ch == 127 || ch == '\b') {
            if (!result.empty()) { result.pop_back(); cout << "\b \b"; }
        } else if (ch >= 32) { result += ch; cout << '*'; }
    }
    cout << '\n';
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif
    return result;
}

string readKey(const char* prompt) {
    cout << prompt;
    string key = readMaskedLine();
    if (key.empty()) throw std::invalid_argument("Key cannot be empty.");
    if (key.size() < 4) {
        cout << "[WARNING] Key is very short (" << key.size()
             << " chars). Short keys are easy to break.\nContinue anyway? (y/n): ";
        char c; cin.get(c); cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (c != 'y' && c != 'Y')
            throw std::invalid_argument("Encryption cancelled. Use a longer key (4+ characters).");
    }
    return key;
}

bool confirmOverwrite(const string& filepath) {
    if (!fs::exists(filepath)) return true;
    cout << "[WARNING] File already exists: " << filepath << "\nOverwrite? (y/n): ";
    char c; cin.get(c); cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return (c == 'y' || c == 'Y');
}
