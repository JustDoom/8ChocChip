#ifndef MISCUTIL_H
#define MISCUTIL_H

#include <string>
#include <string_view>
#include <algorithm>

#include "SFML/Window/Keyboard.hpp"
#include "SFML/Window/Mouse.hpp"

class MiscUtil {
public:
    static std::string toLowerCase(const std::string_view string);

    static std::string getFileFromPath(std::string path);
};

#endif //MISCUTIL_H
