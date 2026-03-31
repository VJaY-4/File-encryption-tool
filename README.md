# File Encryption & Decryption Tool

A **C++17** application that encrypts and decrypts text files and images using the **XOR cipher**. Features both a **GUI** (Dear ImGui) and a **command-line interface**. Image encryption includes an additional byte-scrambling layer for extra obfuscation.

---

## Features

### GUI Application
A clean, minimalistic graphical interface built with **Dear ImGui + GLFW + OpenGL 3.3**. White and black theme with floating particle animations, smooth page transitions, and native file dialogs. Encrypt or decrypt files with just a few clicks — no terminal required.

### Command-Line Interface
The original terminal-based interface with interactive menus, masked key input, and colored output. Ideal for scripting or quick operations.

### Text File Encryption & Decryption
Encrypt and decrypt `.txt` files using XOR cipher. You can either **select an existing file** or **create a new file** by typing content directly. Encrypted files are saved to `Encrypted Files/Text Files/`, and decrypted output goes to `Decrypted Files/Text Files/`.

### Image File Encryption & Decryption
Encrypt and decrypt image files in `.jpg`, `.jpeg`, and `.png` formats. Encrypted images are saved with a `.enc` tag in the filename to `Encrypted Files/Images/`, and decrypted images are restored to `Decrypted Files/Images/`.

### Double-Layer Image Encryption
Image files go through two encryption steps. First, every byte is XOR'd with the key. Then, byte positions are scrambled using a deterministic pseudo-random sequence seeded from the key, making the encrypted image visually unrecognizable. Decryption reverses both steps.

### Key Strength Validation
Short keys (< 4 characters) trigger a warning. You can choose to proceed or use a stronger key.

### Overwrite Protection
The tool checks for existing files before saving and prompts before overwriting.

### Organized File Structure
Required folders are created automatically on startup: `Input Files/`, `Encrypted Files/`, and `Decrypted Files/` with subfolders for text and images.

---

## Project Structure

```
├── gui_main.cpp        # GUI application (Dear ImGui)
├── crypto_core.cpp/h   # Non-interactive encryption API (used by GUI)
├── main.cpp            # CLI entry point and menu system
├── txt_crypt.cpp/h     # Text file encryption & decryption
├── img_crypt.cpp/h     # Image file encryption & decryption
├── utils.cpp/h         # Shared utilities (XOR cipher, key input, etc.)
├── CMakeLists.txt      # CMake build config (builds both CLI & GUI)
├── .gitignore
│
├── libs/
│   ├── imgui/          # Dear ImGui (immediate-mode GUI)
│   ├── glfw/           # GLFW (windowing & input)
│   └── tinyfiledialogs/ # Native OS file dialogs
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
- [CMake](https://cmake.org/) 3.16+
- OpenGL 3.3 capable GPU (for the GUI)

---

## Getting Started

### 1. Clone the repository

```bash
git clone https://github.com/VJaY-4/File-encryption-tool.git
cd File-encryption-tool
```

### 2. Build the project

#### Using CMake (builds both CLI and GUI)

```bash
cmake -S . -B build
cmake --build build
```

> **Windows (MSYS2/MinGW):**
> ```bash
> cmake -S . -B build -G "MinGW Makefiles"
> cmake --build build
> ```

This produces two executables:
- **encrypt_tool** — Command-line interface
- **encrypt_tool_gui** — Graphical interface

#### CLI only (no CMake needed)

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic -o encrypt_tool main.cpp txt_crypt.cpp img_crypt.cpp utils.cpp
```

### 3. Run

```bash
# GUI
./build/encrypt_tool_gui

# CLI
./build/encrypt_tool
```

On Windows, use `.exe` extensions: `encrypt_tool_gui.exe` / `encrypt_tool.exe`.

---

## Usage

### GUI
1. Launch `encrypt_tool_gui`
2. Click **Encrypt** or **Decrypt** on the home screen
3. Select file type (Text or Image)
4. Browse for a file or type new content
5. Enter an encryption key
6. Click the action button — output is saved to the corresponding folder

### CLI
1. Run `encrypt_tool`
2. Choose **Encrypt** or **Decrypt** from the menu
3. Select the file type (Text or Image)
4. Provide the file (enter a path or place it in `Input Files/`)
5. Enter an encryption key (input is masked)
6. Output is saved to the corresponding folder

---

## How It Works

### XOR Encryption

XOR (Exclusive OR) is a bitwise operation where the same operation and key are used for both encryption and decryption:

```
Data XOR Key = Encrypted Data
Encrypted Data XOR Key = Original Data
```

**Text files** — Each byte is XOR'd with the corresponding byte of the key (key repeats cyclically).

**Image files** — Two layers:
1. XOR cipher on every byte of raw image data
2. Byte-position scrambling using a deterministic pseudo-random sequence seeded from the key

### Limitations

> **Note:** XOR with a short, repeating key is **not cryptographically secure**. This tool is a **learning project** to demonstrate encryption concepts — not for protecting sensitive data. For real-world security, use established libraries like OpenSSL or libsodium.

---

## Tech Stack

- **C++17** — Core language
- **Dear ImGui** — Immediate-mode GUI framework
- **GLFW** — Windowing and input
- **OpenGL 3.3** — Rendering backend
- **tinyfiledialogs** — Native OS file dialogs
- **CMake** — Build system

---

This project is open source.
