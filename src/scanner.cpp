#include "scanner.hpp"
#include "utils.hpp"
#include <iostream>
#include <algorithm>

namespace diskscope {

FolderInfo scanFolder(const fs::path& folderPath) {
    // Step 1: Create a FolderInfo for this folder
    FolderInfo folder(folderPath);
    
    // Step 2: Try to create a directory iterator
    // Using error_code instead of exceptions for better performance
    std::error_code ec;
    auto dirIterator = fs::directory_iterator(folderPath, ec);
    
    // Step 3: Check if we have permission to read this folder
    if (ec) {
        folder.accessDenied = true;
        // Note: We don't print here to keep output clean
        // The tree printer will show [ACCESS DENIED]
        return folder;
    }
    
    // Step 4: Loop through each item in the folder
    for (const auto& entry : dirIterator) {
        std::error_code entryEc;
        
        // Skip symbolic links to avoid infinite loops and double-counting
        if (entry.is_symlink(entryEc)) {
            continue;
        }
        
        // Check if this entry is a directory
        if (entry.is_directory(entryEc) && !entryEc) {
            // It's a subfolder - RECURSE!
            // The recursive call will calculate the subfolder's total size
            FolderInfo child = scanFolder(entry.path());
            
            // Add the child's size to our total
            folder.sizeBytes += child.sizeBytes;
            
            // Add child to our children list
            // std::move avoids copying the entire subtree
            folder.children.push_back(std::move(child));
        }
        else if (entry.is_regular_file(entryEc) && !entryEc) {
            // It's a regular file - add its size
            auto fileSize = entry.file_size(entryEc);
            if (!entryEc) {
                folder.sizeBytes += fileSize;
            }
            // If we can't read the file size, we just skip it
        }
        // We ignore other types (sockets, block devices, etc.)
    }
    
    return folder;
}

void sortBySize(FolderInfo& folder) {
    // Sort children by size in descending order (largest first)
    std::sort(folder.children.begin(), folder.children.end(),
        [](const FolderInfo& a, const FolderInfo& b) {
            return a.sizeBytes > b.sizeBytes;
        });
    
    // Recursively sort each child's children
    for (auto& child : folder.children) {
        sortBySize(child);
    }
}

void printTree(const FolderInfo& folder, const std::string& prefix, bool isLast) {
    // Step 1: Determine which branch character to use
    // Using ASCII-compatible characters for cross-platform support
    std::string branch = isLast ? "+-- " : "+-- ";
    
    // Step 2: Print this folder's line
    std::cout << prefix << branch << folder.name;
    
    // Step 3: Add size or access denied status
    if (folder.accessDenied) {
        std::cout << " [ACCESS DENIED]";
    } else {
        std::cout << " [" << formatSize(folder.sizeBytes) << "]";
    }
    std::cout << "\n";
    
    // Step 4: Prepare the prefix for children
    // If this is the last item, children get spaces
    // Otherwise, children get a vertical line to continue the visual
    std::string childPrefix = prefix + (isLast ? "    " : "|   ");
    
    // Step 5: Recursively print each child
    for (size_t i = 0; i < folder.children.size(); ++i) {
        bool childIsLast = (i == folder.children.size() - 1);
        printTree(folder.children[i], childPrefix, childIsLast);
    }
}

void printResults(const FolderInfo& root) {
    // Print a nice header
    std::cout << "\n";
    std::cout << "===========================================\n";
    std::cout << "           DiskScope Results\n";
    std::cout << "===========================================\n\n";
    
    // Print the root folder (special case - no branch character)
    std::cout << root.fullPath.string() << " [" << formatSize(root.sizeBytes) << "]\n";
    
    // Print all children with tree structure
    for (size_t i = 0; i < root.children.size(); ++i) {
        bool isLast = (i == root.children.size() - 1);
        printTree(root.children[i], "", isLast);
    }
    
    std::cout << "\n";
    std::cout << "===========================================\n";
    std::cout << "Total: " << formatSize(root.sizeBytes) << "\n";
    std::cout << "Folders scanned: " << root.children.size() << "\n";
    std::cout << "===========================================\n";
}

} // namespace diskscope
