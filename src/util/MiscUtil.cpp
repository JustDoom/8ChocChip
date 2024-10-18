#include "MiscUtil.h"

std::string MiscUtil::toLowerCase(const std::string_view string) {
    std::string result(string); // Create a mutable string from the string_view
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return result;
}

std::string MiscUtil::getFileFromPath(std::string path) {
    size_t lastSlashPos = path.find_last_of('/');

    if (lastSlashPos != std::string::npos) {
        return path.substr(lastSlashPos + 1);
    }

    return path;
}