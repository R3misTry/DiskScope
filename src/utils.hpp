#ifndef DISKSCOPE_UTILS_HPP
#define DISKSCOPE_UTILS_HPP

#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <cstdint>

namespace diskscope {

/**
 * Converts bytes to human-readable format (KB, MB, GB, TB)
 * 
 * Examples:
 *   formatSize(1024)       -> "1.00 KB"
 *   formatSize(1073741824) -> "1.00 GB"
 */
inline std::string formatSize(std::uintmax_t bytes) {
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    const int numUnits = 5;
    
    int unitIndex = 0;
    double size = static_cast<double>(bytes);
    
    // Keep dividing by 1024 until we get a reasonable number
    while (size >= 1024.0 && unitIndex < numUnits - 1) {
        size /= 1024.0;
        unitIndex++;
    }
    
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << size << " " << units[unitIndex];
    return oss.str();
}

// Forward declaration (FolderInfo is defined in scanner.hpp)
struct FolderInfo;

/**
 * Recursively prints a folder in tree format
 * 
 * @param folder    The folder to print
 * @param prefix    The current line prefix (for indentation)
 * @param isLast    Whether this is the last item in its parent
 */
inline void printTree(const FolderInfo& folder, const std::string& prefix, bool isLast);

/**
 * Prints the complete scan results with a nice header
 * 
 * @param root  The root folder from scanning
 */
inline void printResults(const FolderInfo& root);

} // namespace diskscope

// Include implementation after FolderInfo is defined
// (actual implementation is in scanner.cpp to avoid circular dependency)

#endif // DISKSCOPE_UTILS_HPP
