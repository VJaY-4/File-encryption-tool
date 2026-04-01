#include "img_crypt.h"
#include "utils.h"
#include "crypto_core.h"

#include <iostream>
#include <filesystem>
#include <stdexcept>
#include <algorithm>
#include <limits>

using namespace std;
namespace fs = filesystem;

static string getImagePath() {
    cout << "\n--- Image Location ---\n  1. Enter full path\n  2. Filename in Input Files\nChoice: ";
    int choice;
    if (!(cin >> choice)) {
        cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
        throw invalid_argument("Invalid choice.");
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    string filepath;
    if (choice == 1) { cout << "Enter full path to image: "; getline(cin, filepath); }
    else if (choice == 2) { cout << "Enter image filename (e.g., photo.png): "; getline(cin, filepath); filepath = INPUT_DIR + "/" + filepath; }
    else throw invalid_argument("Invalid choice. Enter 1 or 2.");

    if (filepath.empty()) throw invalid_argument("Path cannot be empty.");
    trimTrailing(filepath);

    string ext = fs::path(filepath).extension().string();
    transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    if (ext != ".jpg" && ext != ".jpeg" && ext != ".png")
        throw invalid_argument("Only .jpg, .jpeg, and .png files are supported.");
    if (!fs::exists(filepath)) throw runtime_error("File not found: " + filepath);
    return filepath;
}

void encryptImage() {
    string filepath = getImagePath();
    string key = readKey("Enter encryption key: ");
    string outpath = encryptImageFile(filepath, key);
    cout << "\n[SUCCESS] Encrypted image saved to: " << outpath << "\n";
}

void decryptImage() {
    cout << "\nEncrypted images in '" << ENC_IMG_DIR << "':\n";
    bool found = false;
    try {
        for (const auto& e : fs::directory_iterator(ENC_IMG_DIR)) {
            string name = e.path().filename().string();
            if (e.is_regular_file() && name.find(".enc.") != string::npos) { cout << "  - " << name << "\n"; found = true; }
        }
    } catch (const fs::filesystem_error&) {}
    if (!found) cout << "  (none)\n";

    cout << "\nEnter encrypted image filename (e.g., photo.enc.png): ";
    string filename; getline(cin, filename);
    trimTrailing(filename);
    if (filename.empty()) throw invalid_argument("Filename cannot be empty.");

    string key = readKey("Enter decryption key: ");
    string outpath = decryptImageFile(ENC_IMG_DIR + "/" + filename, key);
    cout << "\n[SUCCESS] Decrypted image saved to: " << outpath << "\n";
}
