#ifndef MISCUTIL_H
#define MISCUTIL_H

#include "../../dependencies/clay/clay.h"
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

std::string toLowerCase(std::string string);
std::string filesystemPathToString(const std::filesystem::path& path);
std::string getFileFromStringPath(const std::string& path);
bool stringEndsWith(const std::string& str, const std::string& endStr);
void searchDirectory(const std::string& directory, std::unordered_map<std::string*, std::vector<std::string>>& romFiles, std::vector<std::string>& romDirectories);
void handleClayErrors(Clay_ErrorData errorData);
void clearClayStringBuffers();
Clay_String toClayString(const std::string& str);
Clay_Dimensions SDL_MeasureText(Clay_StringSlice text, Clay_TextElementConfig *config, void *userData);
char intToHexCipher(int n);

std::string sha1FromFile(const std::string& filename);

#endif