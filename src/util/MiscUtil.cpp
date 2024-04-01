//
// Created by doom on 3/27/24.
//

#include "MiscUtil.h"

std::string MiscUtil::toLowerCase(const std::string& string) {
    std::string result;

    for (char ch: string) {
        // Convert each character to lowercase using tolower
        result += tolower(ch);
    }

    return result;
}

std::string MiscUtil::getFileFromPath(std::string path) {
    size_t lastSlashPos = path.find_last_of('/');

    if (lastSlashPos != std::string::npos) {
        return path.substr(lastSlashPos + 1);
    }

    return path;
}