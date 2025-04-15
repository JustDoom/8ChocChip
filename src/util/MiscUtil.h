#ifndef MISCUTIL_H
#define MISCUTIL_H

#include "../../dependencies/clay/clay.h"
#include <filesystem>
#include <string>

#include <unordered_map>
#include <vector>

std::string toLowerCase(std::string string);
std::string toString(const std::filesystem::path& path);
std::string getFileFromPath(std::string& path);
void searchDirectory(const std::string& directory, std::unordered_map<std::string*, std::vector<std::string>>& romFiles, std::vector<std::string>& romDirectories);
void handleClayErrors(Clay_ErrorData errorData);
void clearClayStringBuffers();
Clay_String toClayString(const std::string& str);
Clay_Dimensions SDL_MeasureText(Clay_StringSlice text, Clay_TextElementConfig *config, void *userData);

std::string sha1FromFile(const std::string& filename);

#endif