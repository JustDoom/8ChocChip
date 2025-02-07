#ifndef CONSTANTS_H
#define CONSTANTS_H

#ifdef _WIN32
    #define HOME_VARIABLE "USERPROFILE"
#else
    #define HOME_VARIABLE "HOME"
#endif

inline const char* home = std::getenv(HOME_VARIABLE);

#endif
