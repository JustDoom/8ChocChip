#ifndef CONSTANTS_H
#define CONSTANTS_H

#ifdef _WIN32
    #define HOME_VARIABLE "USERPROFILE"
#else
    #define HOME_VARIABLE "HOME"
#endif

inline const char* home = std::getenv(HOME_VARIABLE);
inline std::string configFilePath;
inline bool debug = false;
inline bool speedTest = false;
inline uint64_t ipf = 15;
inline std::unordered_map<uint8_t, unsigned char> defaultKeymap = {
    {30, 0x1}, // 1
    {31, 0x2}, // 2
    {32, 0x3}, // 3
    {33, 0xc}, // 4
    {20, 0x4}, // Q
    {26, 0x5}, // W
    {8, 0x6}, // E
    {21, 0xD}, // R
    {4, 0x7}, // A
    {22, 0x8}, // S
    {7, 0x9}, // D
    {9, 0xE}, // F
    {29, 0xA}, // Z
    {27, 0x0}, // X
    {6, 0xB}, // C
    {25, 0xF}  // V
};
inline int sha1Dimension = 40;

#endif
