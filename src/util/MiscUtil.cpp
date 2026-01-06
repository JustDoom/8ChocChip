#include "MiscUtil.h"

#include <SDL3_ttf/SDL_ttf.h>

#include <algorithm>
#include <cctype>
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

#include "Sha1.h"

std::vector<std::unique_ptr<char[]>> clayStringBuffers;

std::string toLowerCase(std::string string) {
    std::ranges::transform(string, string.begin(), [](const unsigned char c) { return std::tolower(c); });
    return string;
}

std::string filesystemPathToString(const std::filesystem::path& path) {
#ifdef _WIN32
    return path.string();
#else
    return path.string();
#endif
}

std::string getFileFromStringPath(const std::string& path) {
    if (const size_t lastSlashPos = path.find_last_of("/\\"); lastSlashPos != std::string::npos) {
        return path.substr(lastSlashPos + 1);
    }
    return path;
}

bool stringEndsWith(const std::string& str, const std::string& endStr) {
    if (endStr.size() >= str.size()) {
        return false;
    }

    return str.compare(str.size() - endStr.size(), endStr.size(), endStr) == 0;
}

void searchDirectory(const std::string& directory, std::unordered_map<std::string*, std::vector<std::string>>& romFiles,
                     std::vector<std::string>& romDirectories) {
    if (const std::ifstream file(directory); !file.good()) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Unable to find directory %s", directory.c_str());
        return;
    }

    for (const auto& romFile : std::filesystem::directory_iterator(directory)) {
        if (romFile.is_directory()) {
            continue;
        }

        SDL_Log("Processing file: %s - %s", filesystemPathToString(romFile.path()).c_str(),
                sha1FromFile(filesystemPathToString(romFile.path())).c_str());

        if (romFile.file_size() > 3584) {
            continue;
        }

        // Check if the rom directory doesn't exist in romFiles, then add it
        if (!romFiles.contains(&romDirectories.back())) {
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

Clay_Dimensions SDL_MeasureText(Clay_StringSlice text, Clay_TextElementConfig* config, void* userData) {
    auto** fonts = static_cast<TTF_Font**>(userData);
    TTF_Font* font = fonts[config->fontId];
    int width, height;

    if (!TTF_GetStringSize(font, text.chars, text.length, &width, &height)) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to measure text: %s", SDL_GetError());
    }

    return {static_cast<float>(width), static_cast<float>(height)};
}

std::string sha1FromFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        return "";
    }

    SHA1_CTX ctx;
    SHA1Init(&ctx);

    constexpr size_t bufferSize = 4096;
    char buffer[bufferSize];

    while (file.read(buffer, bufferSize) || file.gcount() > 0) {
        SHA1Update(&ctx, reinterpret_cast<const unsigned char*>(buffer), static_cast<uint32_t>(file.gcount()));
    }

    file.close();

    unsigned char hash[20];
    SHA1Final(hash, &ctx);

    std::ostringstream result;
    result << std::hex << std::setfill('0');
    for (unsigned char i : hash) {
        result << std::setw(2) << static_cast<unsigned int>(i);
    }

    return result.str();
}

char intToHexCipher(const int n) {
    if (n <= 9) {
        return '0' + n;
    }
    switch (n) {
    case 0xA:
        return 'A';
    case 0xB:
        return 'B';
    case 0xC:
        return 'C';
    case 0xD:
        return 'D';
    case 0xE:
        return 'E';
    case 0xF:
        return 'F';
    }
    return '?';
}
