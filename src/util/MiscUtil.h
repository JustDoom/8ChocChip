#ifndef MISCUTIL_H
#define MISCUTIL_H

#include <filesystem>
#include <string>
#include "../../dependencies/clay/clay.h"

std::string toLowerCase(std::string string);
std::string to_string(const std::filesystem::path& path);
std::string getFileFromPath(std::string& path);
void handleClayErrors(Clay_ErrorData errorData);
void clearClayStringBuffers();
Clay_String toClayString(const std::string& str);
Clay_Dimensions SDL_MeasureText(Clay_StringSlice text, Clay_TextElementConfig *config, void *userData);

#endif