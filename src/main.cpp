/**
 * DiskScope - A simple disk space analyzer
 * 
 * This program scans a directory and displays the size of each
 * folder in a tree structure, similar to the Linux 'tree' command.
 * 
 * Usage: diskscope [path]
 *   path - Optional. The directory to scan. Defaults to current directory.
 * 
 * Example:
 *   diskscope C:\Users\John\Documents
 *   diskscope .
 */

#include "scanner.hpp"
#include <iostream>
#include <string>

// For Windows console setup
#ifdef _WIN32
#include <windows.h>
#endif

void setupConsole() {
#ifdef _WIN32
    // Enable UTF-8 output on Windows console
    SetConsoleOutputCP(CP_UTF8);
    
    // Enable ANSI escape sequences (for potential future color support)
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hConsole, &mode);
    SetConsoleMode(hConsole, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif
}

void printUsage(const char* programName) {
    std::cout << "\n";
    std::cout << "DiskScope - Disk Space Analyzer\n";
    std::cout << "================================\n\n";
    std::cout << "Usage: " << programName << " [path]\n\n";
    std::cout << "Arguments:\n";
    std::cout << "  path    Directory to scan (default: current directory)\n\n";
    std::cout << "Examples:\n";
    std::cout << "  " << programName << " C:\\Users\\John\\Documents\n";
    std::cout << "  " << programName << " .\n";
    std::cout << "  " << programName << "\n";
}

int main(int argc, char* argv[]) {
    // Setup Windows console for proper character display
    setupConsole();
    
    // Determine the path to scan
    std::string targetPath;
    
    if (argc > 1) {
        // Path provided as command line argument
        targetPath = argv[1];
        
        // Check for help flag
        if (targetPath == "-h" || targetPath == "--help" || targetPath == "/?") {
            printUsage(argv[0]);
            return 0;
        }
    } else {
        // No argument - use current directory
        targetPath = ".";
    }
    
    // Convert to filesystem path and validate
    fs::path scanPath;
    try {
        scanPath = fs::absolute(targetPath);
    } catch (const std::exception& e) {
        std::cerr << "Error: Invalid path '" << targetPath << "'\n";
        std::cerr << "Details: " << e.what() << "\n";
        return 1;
    }
    
    // Check if the path exists
    if (!fs::exists(scanPath)) {
        std::cerr << "Error: Path does not exist: " << scanPath << "\n";
        return 1;
    }
    
    // Check if it's a directory
    if (!fs::is_directory(scanPath)) {
        std::cerr << "Error: Path is not a directory: " << scanPath << "\n";
        return 1;
    }
    
    // Print scanning message
    std::cout << "\nScanning: " << scanPath << "\n";
    std::cout << "Please wait...\n";
    
    // Perform the scan
    diskscope::FolderInfo result = diskscope::scanFolder(scanPath);
    
    // Sort folders by size (largest first)
    diskscope::sortBySize(result);
    
    // Print the results
    diskscope::printResults(result);
    
    return 0;
}
