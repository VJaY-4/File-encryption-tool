#ifndef UTILS_H
#define UTILS_H

#include <string>

// Folder paths.
extern const std::string ENC_TEXT_DIR;   // Encrypted Files/Text Files
extern const std::string ENC_IMG_DIR;    // Encrypted Files/Images
extern const std::string DEC_TEXT_DIR;   // Decrypted Files/Text Files
extern const std::string DEC_IMG_DIR;    // Decrypted Files/Images
extern const std::string INPUT_DIR;      // Input Files (staging folder)

// XOR each byte of buf with the key. Running twice with the same key restores original data.
void xorCipher(char* buf, size_t len, const std::string& key);

// Prompt the user for a key (input is masked with '*'). Validates minimum length.
std::string readKey(const char* prompt);

// Strip trailing whitespace, carriage returns, and newlines from a string.
void trimTrailing(std::string& s);

// Ask "File exists. Overwrite? (y/n)" and return true if user says yes.
bool confirmOverwrite(const std::string& filepath);

#endif // UTILS_H
