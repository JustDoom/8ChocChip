#ifndef MISCUTIL_H
#define MISCUTIL_H

#include <string>
#include <string_view>

class MiscUtil {
public:
    static std::string toLowerCase(std::string_view string);

    static std::string getFileFromPath(std::string path);
};

#endif