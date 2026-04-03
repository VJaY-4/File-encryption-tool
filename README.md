<div align="center">

# 🔐 CryptVault

### Lightweight File Encryption Suite for Windows

**Encrypt and decrypt text files & images with a sleek desktop GUI — powered by C++17.**

[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg?style=flat&logo=c%2B%2B)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.16%2B-064F8C.svg?style=flat&logo=cmake)](https://cmake.org/)
[![OpenGL](https://img.shields.io/badge/OpenGL-3.3-green.svg?style=flat&logo=opengl)](https://www.opengl.org/)
[![License](https://img.shields.io/badge/License-Open%20Source-brightgreen.svg?style=flat)]()

---

<!-- Replace the path below with your actual screenshot -->
<img src="screenshot.png" alt="CryptVault – Home Screen" width="720" />

*Minimalistic white & black interface with floating particle animations and smooth transitions.*

</div>

---

## Why CryptVault?

Most encryption tools are either command-line only or bloated with features you don't need. **CryptVault** gives you a polished desktop experience in under 2 MB — no installers, no dependencies to hunt down, just build and run.

- **Beautiful GUI** — Dear ImGui-powered interface with a clean monochrome aesthetic, rounded corners, and subtle particle animations.
- **Dual Interface** — Use the graphical app or drop down to the CLI for scripting and automation.
- **Text & Image Support** — Encrypt `.txt` files and `.jpg`/`.jpeg`/`.png` images with a single key.
- **Cipher Options** — Choose between **XOR** and **AES-256** in both Encrypt and Decrypt flows.
- **Double-Layer Image Encryption** — XOR cipher + byte-position scrambling makes encrypted images completely unrecognizable.
- **Zero Config** — Output folders are created automatically. Just pick a file, enter a key, and go.

---

## Features

| Feature | Details |
|---|---|
| **GUI Application** | White/black themed desktop app with floating particles, smooth page transitions, native file dialogs |
| **Drag & Drop Input** | Drop supported files directly into the app window on Encrypt/Decrypt pages for faster selection |
| **Save As on Decrypt** | Decrypting text or image files in GUI prompts a native Save As dialog so you choose the exact destination |
| **CLI Interface** | Interactive terminal menus, masked key input, colored output — great for scripting |
| **Cipher Selection** | Supports both XOR and AES-256 for encryption and decryption |
| **Text Encryption** | Encrypt/decrypt `.txt` files or create new encrypted content from scratch |
| **Image Encryption** | Encrypt/decrypt `.jpg`, `.jpeg`, `.png` with dual-layer obfuscation |
| **Key Validation** | Warns on weak keys (< 4 chars) with option to proceed or strengthen |
| **Overwrite Protection** | Prompts before replacing existing files |
| **Auto File Organization** | Creates `Input Files/`, `Encrypted Files/`, `Decrypted Files/` directories on startup |

---

## Quick Start

### Prerequisites

- C++17 compiler (GCC, Clang, or MSVC)
- [CMake 3.16+](https://cmake.org/)
- OpenGL 3.3 capable GPU

### Build

```bash
git clone https://github.com/VJaY-4/File-encryption-tool.git
cd File-encryption-tool
```

**Linux / macOS:**
```bash
cmake -S . -B build
cmake --build build
```

**Windows (MSYS2 / MinGW):**
```bash
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
```

Executables are emitted to the project root for easier double-click launching on Windows.

### Run

```bash
# Launch the GUI
./encrypt_tool_gui        # Linux/macOS
.\encrypt_tool_gui.exe    # Windows

# Launch the CLI
./encrypt_tool            # Linux/macOS
.\encrypt_tool.exe        # Windows
```

> **CLI-only build** (no CMake needed):
> ```bash
> g++ -std=c++17 -Wall -Wextra -Wpedantic -o encrypt_tool main.cpp txt_crypt.cpp img_crypt.cpp utils.cpp
> ```

---

## Usage

### GUI Workflow

```
Launch App  →  Encrypt / Decrypt  →  Text or Image  →  Pick File  →  Enter Key  →  Done ✓
```

1. Open **encrypt_tool_gui**
2. Click **Encrypt** or **Decrypt** on the home screen
3. Choose file type — Text or Image
4. Browse for a file, drag and drop a file into the app, or type new content directly
5. Enter your encryption key
6. For decryption, choose where to save using Save As (or preselect Save As...)
7. Hit the action button — output is saved to your chosen path

### CLI Workflow

1. Run **encrypt_tool**
2. Select **Encrypt** or **Decrypt**
3. Choose file type
4. Provide a file path or place files in `Input Files/`
5. Enter key (input is masked)
6. Output saved to the corresponding folder

---

## How It Works

### XOR Cipher

XOR is a symmetric bitwise operation where the same key encrypts and decrypts.

XOR works as:

```
plaintext  ⊕  key  =  ciphertext
ciphertext ⊕  key  =  plaintext
```

The key repeats cyclically across the data. Simple, fast, and reversible.

### AES-256 Cipher

AES-256 is a stronger block cipher mode for better practical security.

In this project, AES mode uses a derived 256-bit key and encrypted binary output format with embedded metadata required for decryption.

### Double-Layer Image Encryption

Images go through **two passes** for stronger visual obfuscation:

1. **XOR Pass** — Every byte of raw image data is XOR'd with the key
2. **Scramble Pass** — Byte positions are shuffled using a deterministic PRNG seeded from the key

The result is completely unrecognizable. Decryption reverses both steps in order.

> **Disclaimer:** XOR with a short repeating key is not cryptographically secure. AES-256 is the stronger option in this app. CryptVault is a **learning project** demonstrating encryption concepts — not intended for protecting highly sensitive production data.

---

## Project Structure

```
├── gui_main.cpp          GUI application (Dear ImGui + GLFW + OpenGL)
├── crypto_core.cpp/h     Encryption API (non-interactive, used by GUI)
├── main.cpp              CLI entry point
├── txt_crypt.cpp/h       Text file encryption & decryption
├── img_crypt.cpp/h       Image file encryption & decryption
├── utils.cpp/h           Shared utilities (key input, helpers, etc.)
├── CMakeLists.txt        Build configuration
│
├── libs/
│   ├── imgui/            Dear ImGui
│   ├── glfw/             GLFW windowing library
│   ├── stb_image.h       Image loading
│   └── tinyfiledialogs/  Native OS file dialogs
│
├── Input Files/          Drop files here to encrypt (.gitkeep tracked)
├── Encrypted Files/      Encrypted output (Text Files/ & Images/, .gitkeep tracked)
└── Decrypted Files/      Decrypted output (Text Files/ & Images/, .gitkeep tracked)
```

---

## Tech Stack

<div align="center">

| | Technology | Role |
|---|---|---|
| 🧠 | **C++17** | Core language |
| 🖼️ | **Dear ImGui** | Immediate-mode GUI framework |
| 🪟 | **GLFW** | Windowing & input |
| 🎨 | **OpenGL 3.3** | Rendering backend |
| 📂 | **tinyfiledialogs** | Native OS file dialogs |
| 📷 | **stb_image** | Image loading |
| 🔧 | **CMake** | Build system |

</div>

---

<div align="center">

**Built with C++ and curiosity.**

[⬆ Back to Top](#-cryptvault)

</div>
