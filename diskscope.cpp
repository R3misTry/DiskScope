/**
 * DiskScope - Interactive Disk Space Explorer
 * 
 * Navigate through directories and see folder sizes.
 * Uses drill-down navigation instead of printing full tree.
 * 
 * Usage: diskscope [path]
 * Controls: Enter number to navigate, 'b' to go back, 'q' to quit
 */

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cstdint>
#include <future>

#ifdef _WIN32
#include <windows.h>
#endif

namespace fs = std::filesystem;



// ============================================================================
// UTILITIES
// ============================================================================

/**
 * Converts bytes to human-readable format (KB, MB, GB, TB)
 */
std::string formatSize(std::uintmax_t bytes) {
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    const int numUnits = 5;
    
    int unitIndex = 0;
    double size = static_cast<double>(bytes);
    
    while (size >= 1024.0 && unitIndex < numUnits - 1) {
        size /= 1024.0;
        unitIndex++;
    }
    
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << size << " " << units[unitIndex];
    return oss.str();
}

/**
 * Clears the console screen
 */
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// ============================================================================
// SIZE CALCULATION
// ============================================================================

/**
 * Recursively calculates the total size of a folder
 * Does NOT store the tree structure - just returns the size
 */
std::uintmax_t calculateFolderSize(const fs::path& folderPath) {
    std::uintmax_t totalSize = 0;
    std::error_code ec;
    
    // Try to iterate the directory
    auto dirIter = fs::directory_iterator(folderPath, ec);
    if (ec) {
        // Access denied or other error - return 0
        return 0;
    }
    
    for (const auto& entry : dirIter) {
        std::error_code entryEc;
        
        // Skip symbolic links
        if (entry.is_symlink(entryEc)) {
            continue;
        }
        
        if (entry.is_directory(entryEc) && !entryEc) {
            // Recurse into subdirectory
            totalSize += calculateFolderSize(entry.path());
        }
        else if (entry.is_regular_file(entryEc) && !entryEc) {
            // Add file size
            auto fileSize = entry.file_size(entryEc);
            if (!entryEc) {
                totalSize += fileSize;
            }
        }
    }
    
    return totalSize;
}

// ============================================================================
// FOLDER INFO (for current level only)
// ============================================================================

struct FolderEntry {
    std::string name;
    fs::path path;
    std::uintmax_t size;
    bool accessDenied;
};

// Global Cache: Stores folder Scan Results
// Key: full_path_string, Value: list of subfolders
#include <map>
std::map<std::string, std::vector<FolderEntry>> globalCache;

/**
 * Gets immediate subfolders of a directory with their sizes
 * Only calculates ONE level of folders (not recursive listing)
 */
std::vector<FolderEntry> getSubfolders(const fs::path& parentPath) {
    std::vector<FolderEntry> folders;
    std::error_code ec;
    
    auto dirIter = fs::directory_iterator(parentPath, ec);
    if (ec) {
        return folders; // Empty if can't read
    }

    struct Task {
        std::future<std::uintmax_t> future;
        std::string name;
        fs::path path;
    };
    std::vector<Task> tasks;
    
    std::cout << "  Scanning subfolders (Parallel Mode)... " << std::flush;

    for (const auto& entry : dirIter) {
        std::error_code entryEc;
        
        // Only process directories
        if (entry.is_directory(entryEc) && !entryEc && !entry.is_symlink(entryEc)) {
            // Launch async task for each folder
            // std::launch::async forces a new thread (or implementation defined logic)
            tasks.push_back({
                std::async(std::launch::async, calculateFolderSize, entry.path()),
                entry.path().filename().string(),
                entry.path()
            });
        }
    }
    
    // Collect results
    for (auto& task : tasks) {
        FolderEntry folder;
        folder.name = task.name;
        folder.path = task.path;
        folder.accessDenied = false;
        
        // .get() waits for the thread to finish
        folder.size = task.future.get();
        
        folders.push_back(folder);
        // std::cout << "." << std::flush; // Optional: Show progress dots
    }
    
    // Sort by size descending (largest first)
    std::sort(folders.begin(), folders.end(),
        [](const FolderEntry& a, const FolderEntry& b) {
            return a.size > b.size;
        });
    
    return folders;
}

// ============================================================================
// DISPLAY
// ============================================================================

/**
 * Displays the current directory and its subfolders
 */
void displayCurrentLevel(const fs::path& currentPath, const std::vector<FolderEntry>& folders) {
    clearScreen();
    
    std::cout << "============================================================\n";
    std::cout << "  DiskScope - Interactive Disk Explorer\n";
    std::cout << "============================================================\n\n";
    
    std::cout << "Current: " << currentPath.string() << "\n";
    std::cout << "------------------------------------------------------------\n\n";
    
    if (folders.empty()) {
        std::cout << "  (No subfolders found)\n";
    } else {
        // Find max name length for alignment
        size_t maxNameLen = 0;
        for (const auto& f : folders) {
            maxNameLen = std::max(maxNameLen, f.name.length());
        }
        maxNameLen = std::min(maxNameLen, size_t(40)); // Cap at 40 chars
        
        for (size_t i = 0; i < folders.size(); ++i) {
            std::string displayName = folders[i].name;
            if (displayName.length() > 40) {
                displayName = displayName.substr(0, 37) + "...";
            }
            
            std::cout << "  [" << std::setw(2) << i << "] "
                      << std::left << std::setw(maxNameLen + 2) << displayName
                      << std::right << std::setw(12) << formatSize(folders[i].size)
                      << "\n";
        }
    }
    
    std::cout << "\n------------------------------------------------------------\n";
    std::cout << "  [num] = enter | 'b' = back | 'r' = refresh\n";
    std::cout << "------------------------------------------------------------\n";
    std::cout << "> ";
}

// ============================================================================
// DRIVE DETECTION (Windows)
// ============================================================================

/**
 * Gets list of available drives on Windows (C:\, D:\, etc.)
 */
std::vector<fs::path> getAvailableDrives() {
    std::vector<fs::path> drives;
    
#ifdef _WIN32
    // Check drives A-Z
    for (char letter = 'A'; letter <= 'Z'; ++letter) {
        std::string drivePath = std::string(1, letter) + ":\\";
        
        // Check if drive exists
        UINT driveType = GetDriveTypeA(drivePath.c_str());
        if (driveType != DRIVE_NO_ROOT_DIR && driveType != DRIVE_UNKNOWN) {
            drives.push_back(fs::path(drivePath));
        }
    }
#else
    // On Linux/Mac, just use root
    drives.push_back(fs::path("/"));
#endif
    
    return drives;
}

/**
 * Shows drive selection menu and returns selected path
 */
fs::path selectDrive() {
    std::vector<fs::path> drives = getAvailableDrives();
    
    std::cout << "\n============================================================\n";
    std::cout << "  DiskScope - Interactive Disk Explorer\n";
    std::cout << "============================================================\n\n";
    std::cout << "Available drives:\n";
    std::cout << "------------------------------------------------------------\n\n";
    
    for (size_t i = 0; i < drives.size(); ++i) {
        std::cout << "  [" << i << "] " << drives[i].string() << "\n";
    }
    
    std::cout << "\n------------------------------------------------------------\n";
    std::cout << "Select drive number or type a path: ";
    
    std::string input;
    std::getline(std::cin, input);
    
    // Trim whitespace
    while (!input.empty() && isspace(input.front())) input.erase(input.begin());
    while (!input.empty() && isspace(input.back())) input.pop_back();
    
    // Try to parse as number
    try {
        size_t index = std::stoul(input);
        if (index < drives.size()) {
            return drives[index];
        }
    } catch (...) {
        // Not a number - treat as path
    }
    
    // If it looks like a path, use it directly
    if (!input.empty()) {
        return fs::path(input);
    }
    
    // Default to C:\
    return drives.empty() ? fs::path("C:\\") : drives[0];
}

// ============================================================================
// MAIN - INTERACTIVE LOOP
// ============================================================================

void setupConsole() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hConsole, &mode);
    SetConsoleMode(hConsole, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif
}

int main(int argc, char* argv[]) {
    setupConsole();
    
    // Determine starting path
    fs::path currentPath;
    
    if (argc > 1) {
        std::string arg = argv[1];
        if (arg == "-h" || arg == "--help" || arg == "/?") {
            std::cout << "\nDiskScope - Interactive Disk Explorer\n";
            std::cout << "=====================================\n\n";
            std::cout << "Usage: diskscope [path]\n\n";
            std::cout << "Controls:\n";
            std::cout << "  [number]  Navigate into folder\n";
            std::cout << "  b         Go back to parent\n";
            std::cout << "  r         Refresh current folder\n";
            std::cout << "  q         Quit\n";
            return 0;
        }
        currentPath = fs::absolute(arg);
        
        // Validate provided path
        if (!fs::exists(currentPath) || !fs::is_directory(currentPath)) {
            std::cerr << "Error: Invalid directory: " << currentPath << "\n";
            return 1;
        }
    } else {
        // No argument - show drive selection
        currentPath = selectDrive();
        
        if (!fs::exists(currentPath) || !fs::is_directory(currentPath)) {
            std::cerr << "Error: Invalid directory: " << currentPath << "\n";
            return 1;
        }
    }
    
    // Global cache for folder contents
    std::map<std::string, std::vector<FolderEntry>> globalCache;
    // History stack for "Back" button
    std::vector<fs::path> history; // Only store paths
    
    // Main interaction loop
    while (true) {
        
        // 1. SCAN (if not cached)
        bool needsScan = true;
        std::vector<FolderEntry> folders;
        std::string pathKey = currentPath.string();

        if (globalCache.count(pathKey)) {
             // Found in cache! Use it.
             folders = globalCache[pathKey];
             needsScan = false;
        }

        if (needsScan) {
            std::cout << "\nScanning folders...\n"; // Added this line for user feedback
            folders = getSubfolders(currentPath);
            // Save to cache
            globalCache[pathKey] = folders;
        }

        // 2. DISPLAY
        displayCurrentLevel(currentPath, folders);
        
        // 3. INPUT
        std::string input;
        std::getline(std::cin, input);
        
        // Trim
        while (!input.empty() && isspace(input.front())) input.erase(input.begin());
        while (!input.empty() && isspace(input.back())) input.pop_back();

        if (input.empty()) continue;

        // Process Input
        if (input == "b" || input == "B") {
            // BACK
            if (!history.empty()) {
                currentPath = history.back();
                history.pop_back();
            } else {
                // Return to drive selection if at root history
                currentPath = selectDrive();
            }
        }
        else if (input == "r" || input == "R") {
            // REFRESH (Clear cache for this folder)
            globalCache.erase(pathKey);
        }
        else if (input == "q" || input == "Q") { // Added 'q' for quit
            break;
        }
        else {
            // TRY ENTER FOLDER
            try {
                size_t index = std::stoul(input);
                if (index < folders.size()) {
                    // Push current to history
                    history.push_back(currentPath);
                    // Enter new
                    currentPath = folders[index].path;
                } else {
                    std::cout << "Invalid selection. Press Enter to continue...";
                    std::cin.get();
                }
            } catch (...) {
                std::cout << "Invalid input. Press Enter to continue...";
                std::cin.get();
            }
        }
    }
    
    return 0;
}
