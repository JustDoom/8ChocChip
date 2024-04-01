//
// Created by doom on 3/27/24.
//

#ifndef MISCUTIL_H
#define MISCUTIL_H
#include <string>


class MiscUtil {
public:
    static std::string toLowerCase(const std::string& string);

    static std::string getFileFromPath(std::string path);
};



#endif //MISCUTIL_H
