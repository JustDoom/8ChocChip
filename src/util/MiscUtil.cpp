#include "MiscUtil.h"

#include <algorithm>

std::string toLowerCase(const std::string_view string) {
    std::string result(string); // Create a mutable string from the string_view
    std::transform(result.begin(), result.end(), result.begin(),[](const unsigned char c) { return std::tolower(c); });
    return result;
}

std::string getFileFromPath(std::string path) {
    if (const size_t lastSlashPos = path.find_last_of('/'); lastSlashPos != std::string::npos) {
        return path.substr(lastSlashPos + 1);
    }

    return path;
}