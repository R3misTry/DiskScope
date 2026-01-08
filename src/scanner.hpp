#ifndef DISKSCOPE_SCANNER_HPP
#define DISKSCOPE_SCANNER_HPP

#include <string>
#include <vector>
#include <filesystem>
#include <cstdint>

namespace fs = std::filesystem;

namespace diskscope {

/**
 * Holds information about a single folder
 * 
 * This structure forms a tree: each FolderInfo contains
 * a vector of child FolderInfo objects (its subfolders).
 */
struct FolderInfo {
    std::string name;                    // Folder name (e.g., "Documents")
    fs::path fullPath;                   // Full path (e.g., "C:\Users\John\Documents")
    std::uintmax_t sizeBytes;            // Total size including all subfolders
    std::vector<FolderInfo> children;    // Subfolders (creates tree structure)
    bool accessDenied;                   // True if we couldn't read this folder
    
    /**
     * Constructor - initializes a FolderInfo from a path
     */
    explicit FolderInfo(const fs::path& path)
        : name(path.filename().string())
        , fullPath(path)
        , sizeBytes(0)
        , accessDenied(false)
    {
        // Handle root paths like "C:\" which have empty filename
        if (name.empty()) {
            name = path.string();
        }
    }
    
    // Default constructor for containers
    FolderInfo() : sizeBytes(0), accessDenied(false) {}
};

/**
 * Recursively scans a folder and all its subfolders
 * 
 * This function:
 * 1. Creates a FolderInfo for the given path
 * 2. Iterates through all items in the folder
 * 3. For subfolders: recursively calls itself
 * 4. For files: adds their size to the total
 * 5. Handles permission errors gracefully
 * 
 * @param folderPath  The path to scan
 * @return FolderInfo containing the complete tree structure
 */
FolderInfo scanFolder(const fs::path& folderPath);

/**
 * Sorts all folders by size (largest first)
 * 
 * @param folder  The root folder to sort (modified in place)
 */
void sortBySize(FolderInfo& folder);

/**
 * Prints a folder tree to the console
 * 
 * @param folder  The folder to print
 * @param prefix  Current indentation prefix
 * @param isLast  Whether this is the last child of its parent
 */
void printTree(const FolderInfo& folder, const std::string& prefix, bool isLast);

/**
 * Prints the complete results with a header
 * 
 * @param root  The root folder from scanning
 */
void printResults(const FolderInfo& root);

} // namespace diskscope

#endif // DISKSCOPE_SCANNER_HPP
