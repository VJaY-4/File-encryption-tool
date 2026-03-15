#ifndef TXT_CRYPT_H
#define TXT_CRYPT_H

#include <string>

// Folder paths.
extern const std::string ENC_TEXT_DIR;   // Encrypted Files/Text Files
extern const std::string ENC_IMG_DIR;    // Encrypted Files/Images
extern const std::string DEC_TEXT_DIR;   // Decrypted Files/Text Files
extern const std::string DEC_IMG_DIR;    // Decrypted Files/Images
extern const std::string INPUT_DIR;      // Input Files (staging folder)

// Encrypt a text file: prompts for filename, message, and key.
void encryptFile();

// Decrypt a text file: lists available files, prompts for filename and key.
void decryptFile();

#endif // TXT_CRYPT_H
