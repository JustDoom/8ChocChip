#include "MiscUtil.h"

#include <algorithm>
#include <cctype>

std::string toLowerCase(std::string string) {
    std::transform(string.begin(), string.end(), string.begin(), [](const unsigned char c) { return std::tolower(c); });
    return string;
}

std::string getFileFromPath(std::string& path) {
    if (const size_t lastSlashPos = path.find_last_of('/'); lastSlashPos != std::string::npos) {
        return path.substr(lastSlashPos + 1);
    }

    return path;
}
