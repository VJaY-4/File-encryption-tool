#ifndef CRYPTO_CORE_H
#define CRYPTO_CORE_H

#include <string>
#include <vector>
#include <atomic>

// Cipher method selection.
enum class CipherMethod { XOR, AES256 };

// Folder paths.
extern const std::string ENC_TEXT_DIR;
extern const std::string ENC_IMG_DIR;
extern const std::string DEC_TEXT_DIR;
extern const std::string DEC_IMG_DIR;
extern const std::string INPUT_DIR;

// Ensure all output directories exist.
void ensureDirectories();

// XOR cipher: each byte XORed with key (wrapping). Running twice restores original.
void xorCipher(char* buf, size_t len, const std::string& key);

// Strip trailing whitespace / CR / LF.
void trimTrailing(std::string& s);

// ── Text operations (non-interactive, return status string) ─────────

// Encrypt text from a string (new file mode). Returns output path on success.
std::string encryptTextFromString(const std::string& content,
                                  const std::string& filename,
                                  const std::string& key,
                                  CipherMethod method = CipherMethod::XOR,
                                  std::atomic<float>* progress = nullptr);

// Encrypt an existing text file. Returns output path on success.
std::string encryptTextFile(const std::string& inputPath,
                            const std::string& key,
                            CipherMethod method = CipherMethod::XOR,
                            std::atomic<float>* progress = nullptr);

// Decrypt a text file. Returns the decrypted content as a string, saves to Decrypted Files/.
std::string decryptTextFile(const std::string& encryptedPath,
                            const std::string& key,
                            std::string& savedPath,
                            CipherMethod method = CipherMethod::XOR,
                            std::atomic<float>* progress = nullptr);

// ── Image operations (non-interactive) ──────────────────────────────

// Encrypt an image file. Returns output path on success.
std::string encryptImageFile(const std::string& inputPath,
                             const std::string& key,
                             CipherMethod method = CipherMethod::XOR,
                             std::atomic<float>* progress = nullptr);

// Decrypt an image file. Returns output path on success.
std::string decryptImageFile(const std::string& encryptedPath,
                             const std::string& key,
                             CipherMethod method = CipherMethod::XOR,
                             std::atomic<float>* progress = nullptr);

// ── Listing helpers ─────────────────────────────────────────────────

std::vector<std::string> listEncryptedTextFiles();
std::vector<std::string> listEncryptedImageFiles();

#endif // CRYPTO_CORE_H
