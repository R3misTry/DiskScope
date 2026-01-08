# DiskScope 🔍

A lightweight, interactive disk space analyzer for Windows. Navigate through your drives and folders to find what's taking up space.

![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)
![Windows](https://img.shields.io/badge/Platform-Windows-lightgrey.svg)
![License](https://img.shields.io/badge/License-MIT-green.svg)

## Features

- 🖥️ **Interactive navigation** — Browse folders like a file explorer
- 📊 **Size calculation** — See total size of each folder
- 💾 **Drive detection** — Automatically finds your drives (C:\, D:\, etc.)
- ⚡ **Smart caching** — Going back is instant
- 🔄 **Refresh** — Press 'r' to rescan

## Demo

```
Current: C:\Users\John
------------------------------------------------------------

  [ 0] AppData          76.87 GB
  [ 1] Desktop          29.49 GB
  [ 2] Documents         1.67 GB
  [ 3] Downloads       512.30 MB

------------------------------------------------------------
  [num] = enter | 'b' = back | 'r' = refresh
------------------------------------------------------------
>
```

## Installation

### Download

1. Go to [Releases](../../releases)
2. Download `diskscope.exe`
3. Run it!

### Build from source

```bash
g++ -std=c++17 -O2 -static diskscope.cpp -o diskscope.exe
```

## Usage

Run the program and select a drive:

```bash
diskscope.exe              # Shows drive selection menu
diskscope.exe C:\Users     # Scan a specific folder
```

### Controls

| Key    | Action       |
| ------ | ------------ |
| `0-99` | Enter folder |
| `b`    | Go back      |
| `r`    | Refresh      |

## License

MIT
