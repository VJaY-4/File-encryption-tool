/*
 * XOR Cipher File Encryption / Decryption Tool
 * Build: g++ -std=c++17 -Wall -Wextra -o encrypt_tool main.cpp txt_crypt.cpp
 */

#include "txt_crypt.h"
#include "img_crypt.h"
#include "utils.h"

#include <iostream>
#include <filesystem>
#include <limits>

using namespace std;
namespace fs = filesystem;

static int readInt() {
    int v;
    if (!(cin >> v)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return -1;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return v;
}

int main() {
    cout << "+================================================+\n"
         << "|    XOR Cipher File Encryption / Decryption     |\n"
         << "+================================================+\n";

    while (true) {
        cout << "\n--- Menu ---\n  1. Encrypt\n  2. Decrypt\n  3. Exit\nChoice: ";
        int choice = readInt();
        if (choice == -1) { cout << "[ERROR] Enter 1, 2, or 3.\n"; continue; }

        try {
            if (choice == 1 || choice == 2) {
                cout << "\n--- File Type ---\n  1. Text File\n  2. Image File (.jpg/.png)\nChoice: ";
                int type = readInt();
                if (type == -1) { cout << "[ERROR] Enter 1 or 2.\n"; continue; }
                if (choice == 1) {
                    if (type == 1) encryptFile();
                    else if (type == 2) encryptImage();
                    else cout << "[ERROR] Enter 1 or 2.\n";
                } else {
                    if (type == 1) decryptFile();
                    else if (type == 2) decryptImage();
                    else cout << "[ERROR] Enter 1 or 2.\n";
                }
            } else if (choice == 3) {
                cout << "\nGoodbye!\n"; return 0;
            } else {
                cout << "[ERROR] Enter 1, 2, or 3.\n";
            }
        } catch (const exception& e) {
            cerr << "\n[ERROR] " << e.what() << "\n";
        }
    }
}
