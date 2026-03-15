# File Encryption & Decryption Tool

A command-line tool built in **C++17** that encrypts and decrypts text files and images using the **XOR cipher**. Image encryption includes an additional byte-scrambling layer for extra obfuscation.

---

## Features

- **Text file encryption/decryption** — encrypt existing `.txt` files or type content directly into the tool
- **Image file encryption/decryption** — supports `.jpg`, `.jpeg`, and `.png` files
- **Double-layer image encryption** — XOR cipher + byte-position scrambling
- **Masked key input** — keys are hidden with `*` characters while typing
- **Key strength validation** — warns if the encryption key is shorter than 4 characters
- **Overwrite protection** — prompts before overwriting existing output files
- **Organized file structure** — automatically creates separate folders for input, encrypted, and decrypted files

---

## Project Structure

```
├── main.cpp          # Entry point and menu system
├── txt_crypt.cpp/h   # Text file encryption & decryption
├── img_crypt.cpp/h   # Image file encryption & decryption
├── utils.cpp/h       # Shared utilities (XOR cipher, key input, etc.)
├── CMakeLists.txt    # CMake build configuration
├── .gitignore
│
├── Input Files/           # Place files here to encrypt
├── Encrypted Files/
│   ├── Text Files/        # Encrypted text output
│   └── Images/            # Encrypted image output
└── Decrypted Files/
    ├── Text Files/        # Decrypted text output
    └── Images/            # Decrypted image output
```

---

## Prerequisites

- A C++17 compatible compiler (GCC, Clang, or MSVC)
- [CMake](https://cmake.org/) 3.16+ *(optional — you can compile manually)*

---

## Getting Started

### 1. Clone the repository

```bash
git clone https://github.com/VJaY-4/File-encryption-tool.git
cd File-encryption-tool
```

### 2. Build the project

#### Option A: Using g++ directly

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic -o encrypt_tool main.cpp txt_crypt.cpp img_crypt.cpp utils.cpp
```

#### Option B: Using CMake

```bash
cmake -S . -B build
cmake --build build
```

> **Windows (MSYS2/MinGW):** Specify the generator:
> ```bash
> cmake -S . -B build -G "MinGW Makefiles"
> cmake --build build
> ```

### 3. Run the tool

```bash
# If built with g++
./encrypt_tool

# If built with CMake
./build/encrypt_tool
```

On Windows, use `encrypt_tool.exe` or `.\build\encrypt_tool.exe` instead.

---

## Usage

1. Run the executable
2. Choose **Encrypt** or **Decrypt** from the menu
3. Select the file type (Text or Image)
4. Provide the file (enter a path or place it in the `Input Files/` folder)
5. Enter an encryption key (input is masked)
6. The output is saved to the corresponding folder

### Example

```
+================================================+
|    XOR Cipher File Encryption / Decryption     |
+================================================+

--- Menu ---
  1. Encrypt
  2. Decrypt
  3. Exit
Choice: 1

--- File Type ---
  1. Text File
  2. Image File (.jpg/.png)
Choice: 1

--- Source ---
  1. Create a new file (type content)
  2. Select an existing file
Choice: 2

Enter filename (e.g., test1.txt): myfile.txt
Enter encryption key: ****

[SUCCESS] Saved to: Encrypted Files/Text Files/myfile.txt
```

---

## How It Works

### What is XOR Encryption?

XOR (Exclusive OR) is a fundamental bitwise operation in computer science. It compares two bits and returns `1` if they are different, `0` if they are the same:

| A | B | A XOR B |
|---|---|---------|
| 0 | 0 |    0    |
| 0 | 1 |    1    |
| 1 | 0 |    1    |
| 1 | 1 |    0    |

The key property that makes XOR useful for encryption is that **it is its own inverse**:

```
Data XOR Key = Encrypted Data
Encrypted Data XOR Key = Original Data
```

This means the same operation (and the same key) is used for both encryption and decryption, making it a **symmetric cipher**.

### Why XOR Matters

- **Foundation of modern cryptography** — XOR is a core building block used inside industry-standard algorithms like AES, DES, and ChaCha20. Understanding XOR encryption gives insight into how real-world encryption works at the bit level.
- **Perfect secrecy (in theory)** — When used with a truly random key that is as long as the message and never reused, XOR encryption becomes the **One-Time Pad**, which is mathematically proven to be unbreakable. This is the only cipher with a formal proof of perfect secrecy.
- **Simplicity and speed** — XOR operates directly on bits, making it extremely fast. It requires no complex math — just a single CPU instruction per byte.
- **Reversibility** — The self-inverting property (`A XOR B XOR B = A`) makes implementation straightforward with no need for separate encrypt/decrypt logic.

### How This Tool Uses XOR

**Text files** — Each byte of the file is XOR'd with the corresponding byte of the key. The key repeats cyclically if it is shorter than the file:

```
File bytes:    H   e   l   l   o
Key bytes:     k   e   y   k   e   (key "key" repeats)
Result:        XOR of each pair → encrypted bytes
```

**Image files** — Encryption is applied in two layers for stronger obfuscation:
1. **XOR cipher** on every byte of the raw image data
2. **Byte-position scrambling** — bytes are rearranged using a deterministic pseudo-random sequence seeded from the key, making the image visually unrecognizable

Decryption reverses both steps using the same key.

### Limitations

> **Note:** While XOR is the foundation of strong encryption, using it alone with a short, repeating key (as this tool does) is **not cryptographically secure**. Patterns in the original data can leak through, and the key can be recovered through frequency analysis. This tool is intended as a **learning project** to demonstrate encryption concepts — not for protecting sensitive data. For real-world security, use established libraries like OpenSSL or libsodium.

---

## License

This project is open source and available under the [MIT License](LICENSE).