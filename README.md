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

## Build & Run

### Option 1: Using g++ directly

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic -o encrypt_tool main.cpp txt_crypt.cpp img_crypt.cpp utils.cpp
./encrypt_tool
```

### Option 2: Using CMake

```bash
cmake -S . -B build
cmake --build build
./build/encrypt_tool
```

> **Windows note:** If using MSYS2/MinGW, specify the generator:
> ```bash
> cmake -S . -B build -G "MinGW Makefiles"
> cmake --build build
> ```

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

- **Text files** are encrypted using a simple XOR cipher — each byte is XOR'd with the corresponding byte of the key (repeating cyclically).
- **Image files** go through two steps:
  1. XOR cipher on every byte
  2. Byte-position scrambling using a deterministic pseudo-random sequence derived from the key
- Decryption reverses the process using the same key.

> **Note:** XOR encryption is a symmetric cipher — the same key is used for both encryption and decryption. This tool is intended for learning purposes and is not suitable for securing sensitive data.

---

## License

This project is open source and available under the [MIT License](LICENSE).