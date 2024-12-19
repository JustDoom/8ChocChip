#ifndef MISCUTIL_H
#define MISCUTIL_H

#include <string>
#include <string_view>

std::string toLowerCase(std::string_view string);
std::string getFileFromPath(std::string path);

#endif