#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>
#include <filesystem>

#ifdef _WIN32
    #define HOME_VARIABLE "USERPROFILE"
#else
    #define HOME_VARIABLE "HOME"
#endif

inline const char* home = std::getenv(HOME_VARIABLE);
inline std::string configFilePath;
inline std::string databasesFolderPath = std::filesystem::current_path() / ".." / ".." / "dependencies" / "chip-8-database" / "database";
inline bool debug = false;
inline bool speedTest = false;
inline uint64_t ipf = 15;

#endif
