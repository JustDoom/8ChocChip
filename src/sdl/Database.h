#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>
#include <map>

#include "../Settings.h"

struct PlatformData {
    std::string id;
    std::string name;
    std::string description;
    std::string release;
    std::vector<std::string> displayResolutions;
    int defaultTickrate;
    Quirks quirks;
};

struct RomData {
    std::string fileName;
    std::vector<std::string> platforms;
};

struct ProgramData {
    std::string title;
    std::string description;
    std::string release;
    std::vector<std::string> authors;
    std::map<std::string, RomData> roms;
};

class Database {
private:
    std::vector<PlatformData> platforms;
    std::vector<ProgramData> programs;

    void loadPlatforms();
    void loadPrograms();
public:
    Database();

    ProgramData getProgramData(std::string sha1);
    RomData getRomData(std::string sha1);
    std::vector<std::string> getRomPlatforms(std::string sha1);
    PlatformData getPlatformData(std::string platformId);
};

#endif