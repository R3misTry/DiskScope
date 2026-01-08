# DiskScope 🔍

A lightweight, interactive disk space analyzer for Windows. Navigate through your drives and folders to find what's taking up space.

![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)
![Windows](https://img.shields.io/badge/Platform-Windows-lightgrey.svg)
![License](https://img.shields.io/badge/License-MIT-green.svg)

## Features

- 🖥️ **Interactive navigation** — Browse folders like a file explorer
- 📊 **Size calculation** — See total size of each folder (including subfolders)
- 💾 **Drive detection** — Automatically detects available drives (C:\, D:\, etc.)
- ⚡ **Smart caching** — Going back is instant (no rescan needed)
- 🔄 **Refresh on demand** — Press 'r' to rescan current folder
- 🛡️ **Error handling** — Gracefully handles permission-denied folders

## Screenshot

```
============================================================
  DiskScope - Interactive Disk Explorer
============================================================

Current: C:\Users\John
------------------------------------------------------------

  [ 0] AppData                      76.87 GB
  [ 1] Desktop                      29.49 GB
  [ 2] Documents                     1.67 GB
  [ 3] Downloads                   512.30 MB
  [ 4] Pictures                    156.20 MB

------------------------------------------------------------
  [num] = enter | 'b' = back | 'r' = refresh
------------------------------------------------------------
>
```

## Usage

### Download & Run

1. Download `diskscope.exe` from [Releases](../../releases)
2. Double-click to run
3. Select a drive and start exploring!

### Controls

| Key    | Action                          |
| ------ | ------------------------------- |
| `0-99` | Enter the folder at that index  |
| `b`    | Go back to parent folder        |
| `r`    | Refresh (rescan current folder) |
| `X`    | Close the window to exit        |

### Command Line

```bash
# Run with drive selection menu
diskscope.exe

# Scan a specific path directly
diskscope.exe C:\Users\John\Documents

# Show help
diskscope.exe --help
```

## Building from Source

### Requirements

- C++17 compiler (g++ 8+, MSVC 2017+, or Clang 7+)
- Windows 10/11

### With g++ (MinGW/MSYS2)

```bash
g++ -std=c++17 -O2 -static diskscope.cpp -o diskscope.exe
```

### With MSVC

```bash
cl /EHsc /O2 /std:c++17 diskscope.cpp
```

## How It Works

1. **Drive Detection** — Uses Windows API (`GetDriveTypeA`) to find available drives
2. **Size Calculation** — Recursively walks directories using `std::filesystem`
3. **Caching** — Stores visited folder data in a history stack for instant back-navigation
4. **Display** — Sorts folders by size (largest first) for easy identification

## Project Structure

```
DiskScope/
├── diskscope.cpp    # All source code (single file)
├── diskscope.exe    # Compiled executable
└── README.md        # This file
```

## Technical Details

- **Language**: C++17
- **Dependencies**: None (uses only standard library + Windows API)
- **Binary size**: ~200KB (static build)
- **Memory usage**: Minimal (only current level + history cached)

## License

MIT License — feel free to use, modify, and distribute.

## Contributing

Pull requests welcome! Some ideas for improvements:

- [ ] Show file count per folder
- [ ] Export results to file
- [ ] Color-coded output based on size
- [ ] Linux/macOS support
