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

### Command Line

```bash
diskscope.exe              # Run with drive selection
diskscope.exe C:\Users     # Scan a specific path
diskscope.exe --help       # Show help
```

## Building from Source

```bash
g++ -std=c++17 -O2 -static diskscope.cpp -o diskscope.exe
```

## License

MIT License — feel free to use, modify, and distribute.
