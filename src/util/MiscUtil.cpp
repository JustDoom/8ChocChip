#include "MiscUtil.h"

#include <SDL3_ttf/SDL_ttf.h>

#include <algorithm>
#include <cctype>
#include <cstring>
#include <iostream>
#include <memory>
#include <vector>

std::vector<std::unique_ptr<char[]>> clayStringBuffers;

std::string toLowerCase(std::string string) {
    std::transform(string.begin(), string.end(), string.begin(), [](const unsigned char c) { return std::tolower(c); });
    return string;
}

std::string to_string(const std::filesystem::path& path) {
    #ifdef _WIN32
        return path.string();
    #else
        return path.string();
    #endif
}

std::string getFileFromPath(std::string& path) {
  if (const size_t lastSlashPos = path.find_last_of("/\\"); lastSlashPos != std::string::npos) {
        return path.substr(lastSlashPos + 1);
    }
    return path;
}

void searchDirectory(const std::string& directory, std::unordered_map<std::string*, std::vector<std::string>>& romFiles, std::vector<std::string>& romDirectories) {
    for (const auto& romFile: std::filesystem::directory_iterator(directory)) {
        if (romFile.is_directory() || romFile.file_size() > 3584) {
            continue;
        }

        std::cout << "Processing file: " << to_string(romFile.path()) << std::endl;

        // Check if the rom directory doesn't exist in romFiles, then add it
        if (romFiles.find(&romDirectories.back()) == romFiles.end()) {
            romFiles.emplace(&romDirectories.back(), std::vector<std::string>());
        }

        // Add the file path to the romFiles entry
        romFiles.find(&romDirectories.back())->second.emplace_back(romFile.path().string());
    }
}

void handleClayErrors(Clay_ErrorData errorData) {
    printf("%s", errorData.errorText.chars);
}

void clearClayStringBuffers() {
    clayStringBuffers.clear();
}

Clay_String toClayString(const std::string& str) {
    const size_t requiredSize = str.size() + 1;
    auto buffer = std::make_unique<char[]>(requiredSize);
    std::strcpy(buffer.get(), str.c_str());

    const Clay_String clayStr = {
        .length = static_cast<int32_t>(str.size()),
        .chars = buffer.get()
    };

    clayStringBuffers.push_back(std::move(buffer));
    return clayStr;
}

Clay_Dimensions SDL_MeasureText(Clay_StringSlice text, Clay_TextElementConfig *config, void *userData) {
    TTF_Font **fonts = (TTF_Font**) userData;
    TTF_Font *font = fonts[config->fontId];
    int width, height;

    if (!TTF_GetStringSize(font, text.chars, text.length, &width, &height)) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to measure text: %s", SDL_GetError());
    }

    return (Clay_Dimensions) { (float) width, (float) height };
}