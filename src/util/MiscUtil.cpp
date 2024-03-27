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
