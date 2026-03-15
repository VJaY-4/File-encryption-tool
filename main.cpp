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

int main() {
    // Show the program title when it starts.
    // Also create the output folder right away so it is ready to use.
    cout << "+================================================+\n"
         << "+================================================+\n"
         << "|    XOR Cipher File Encryption / Decryption     |\n"
         << "+================================================+\n"
         << "+================================================+\n";

    try {
        fs::create_directories(ENC_TEXT_DIR);
        fs::create_directories(ENC_IMG_DIR);
        fs::create_directories(DEC_TEXT_DIR);
        fs::create_directories(DEC_IMG_DIR);
        fs::create_directories(INPUT_DIR);
    }
    catch (const fs::filesystem_error& e) {
        cerr << "Warning: " << e.what() << "\n";
    }

    // Keep showing the menu until the user picks Exit.
    // Errors are caught and printed so the program does not crash (CO5).
    int choice = 0;
    while (true) {
        cout << "\n--- Menu ---\n"
             << "  1. Encrypt\n  2. Decrypt\n  3. Exit\nChoice: ";

        // Check that the user typed a number, not letters or symbols.
        // If the input is bad, clear it and ask again.
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "[ERROR] Enter 1, 2, or 3.\n";
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        // Run the chosen action and catch any errors (CO5: exception handling).
        // If something goes wrong, print the error and go back to the menu.
        try {
            switch (choice) {
                case 1: {
                    cout << "\n--- File Type ---\n"
                         << "  1. Text File\n"
                         << "  2. Image File (.jpg/.png)\n"
                         << "Choice: ";
                    int type;
                    if (!(cin >> type)) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "[ERROR] Enter 1 or 2.\n";
                        break;
                    }
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    if (type == 1) encryptFile();
                    else if (type == 2) encryptImage();
                    else cout << "[ERROR] Enter 1 or 2.\n";
                    break;
                }
                case 2: {
                    cout << "\n--- File Type ---\n"
                         << "  1. Text File\n"
                         << "  2. Image File (.jpg/.png)\n"
                         << "Choice: ";
                    int type;
                    if (!(cin >> type)) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "[ERROR] Enter 1 or 2.\n";
                        break;
                    }
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    if (type == 1) decryptFile();
                    else if (type == 2) decryptImage();
                    else cout << "[ERROR] Enter 1 or 2.\n";
                    break;
                }
                case 3: cout << "\nGoodbye!\n"; return 0;
                default: cout << "[ERROR] Enter 1, 2, or 3.\n";
            }
        } catch (const exception& e) {
            cerr << "\n[ERROR] " << e.what() << "\n";
        }
    }
}
