#include "Database.h"

#include "../util/Constants.h"

#include <iostream>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <vector>

Database::Database() {
    if (!std::filesystem::exists(databasesFolderPath)) {
        std::cerr << "Unable to find database folder at " << databasesFolderPath << std::endl;
        return;
    }

    loadPlatforms();
    loadPrograms();
}

void Database::loadPlatforms() {
    std::string platformsDatabasePath = (std::filesystem::path(databasesFolderPath) / "platforms.json").string();

    nlohmann::json json;
    if (std::ifstream file(platformsDatabasePath); file.good()) {
        json = nlohmann::json::parse(file);
        file.close();
    }

    for (const auto rawPlatformData : json) {
        PlatformData data;
        data.id = rawPlatformData.at("id");
        data.name = rawPlatformData.at("name");
        data.description = rawPlatformData.at("description");
        if (rawPlatformData.find("release") != rawPlatformData.end()) {
            data.release = rawPlatformData.at("release");
        }
        data.displayResolutions = rawPlatformData.at("displayResolutions");
        data.defaultTickrate = rawPlatformData.at("defaultTickrate");

        data.quirks.shift = rawPlatformData.at("quirks").at("shift");
        if (rawPlatformData.at("quirks").find("memoryIncrementByX") != rawPlatformData.at("quirks").end()) {
            data.quirks.memoryIncrementByX = rawPlatformData.at("quirks").at("memoryIncrementByX");
        }
        data.quirks.memoryLeaveIUnchanged = rawPlatformData.at("quirks").at("memoryLeaveIUnchanged");
        data.quirks.wrap = rawPlatformData.at("quirks").at("wrap");
        data.quirks.jump = rawPlatformData.at("quirks").at("jump");
        data.quirks.vblank = rawPlatformData.at("quirks").at("vblank");
        data.quirks.logic = rawPlatformData.at("quirks").at("logic");
        
        this->platforms.emplace_back(data);
    }
}

void Database::loadPrograms() {
    std::string programsDatabasePath = (std::filesystem::path(databasesFolderPath) / "programs.json").string();

    nlohmann::json json;
    if (std::ifstream file(programsDatabasePath); file.good()) {
        json = nlohmann::json::parse(file);
        file.close();
    }

    for (const auto rawProgramData : json) {
        ProgramData data;
        data.title = rawProgramData.at("title");
        if (rawProgramData.find("description") != rawProgramData.end()) {
            data.description = rawProgramData.at("description");
        }
        if (rawProgramData.find("release") != rawProgramData.end()) {
            data.release = rawProgramData.at("release");
        }
        if (rawProgramData.find("authors") != rawProgramData.end()) {
            data.authors = rawProgramData.at("authors");
        }
        for (auto it = rawProgramData.at("roms").begin(); it != rawProgramData.at("roms").end(); it++) {
            RomData romData;
            romData.fileName = it.value().at("file");
            romData.platforms = it.value().at("platforms");
            data.roms.insert({it.key(), romData});
        }

        this->programs.push_back(data);
    }
}

ProgramData Database::getProgramData(std::string sha1) {
    for (ProgramData programData : this->programs) {
        for (auto it = programData.roms.begin(); it != programData.roms.end(); it++) {
            if (it->first == sha1) {
                return programData;
            }
        }
    }

    return ProgramData{};
}

RomData Database::getRomData(std::string sha1) {
    for (ProgramData programData : this->programs) {
        for (auto it = programData.roms.begin(); it != programData.roms.end(); it++) {
            if (it->first == sha1) {
                return it->second;
            }
        }
    }

    return RomData{};
}

std::vector<std::string> Database::getRomPlatforms(std::string sha1) {
    std::vector<PlatformData> platformsData;
    for (ProgramData programData : this->programs) {
        for (auto it = programData.roms.begin(); it != programData.roms.end(); it++) {
            if (it->first == sha1) {
                return it->second.platforms;
            }
        }
    }

    return { "originalChip8" };
}

PlatformData Database::getPlatformData(std::string platformId) {
    for (PlatformData platform : this->platforms) {
        if (platform.id == platformId) {
            return platform;
        }
    }

    return PlatformData{};
}
