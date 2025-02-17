#include <filesystem>
#include <fstream>
#include <iostream>
#include <unordered_map>

#include <nlohmann/json.hpp>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "Timer.h"
#include "sdl/Emulator.h"
#include "sdl/MainMenu.h"
#include "util/Constants.h"
#include "util/MiscUtil.h"

int main(int argc, char **argv) {
    std::string rom;
    for (int i = 0; i < argc; i++) {
        std::string_view arg = argv[i];
        if (arg.rfind("--") != 0 && arg.rfind("-") != 0) {
            continue;
        }

        std::string command = toLowerCase(arg);
        if (command == "--rom" || command == "-r") {
            if (i + 1 < argc) {
                rom = argv[++i];
                continue;
            }

            std::cerr << "Please include the path to the file" << std::endl;
            return 0;
        }
        if (command == "--help" || command == "-h") {
            std::cerr << "Usage: 8ChocChip --rom <pathtorom>" << std::endl;
            return 0;
        }
        if (command == "--debug" || command == "-d") {
            debug = true;
        }
        if (command == "--speedtest" || command == "-st") {
            std::cout << "speed" << std::endl;
            speedTest = true;
        }
        if (command == "--instructions" || command == "-ipf") {
            std::cout << argv[++i] << std::endl;
            ipf = std::stoi(argv[i]);
        }
    }

    if (!home) {
        std::cerr << home << " environment variable not set. " << std::endl;
        return 1;
    }

    std::string configFilePath = (std::filesystem::path(home) / ".8chocchip.json").string();

    std::vector<std::string> romDirectories;
    std::unordered_map<std::string*, std::vector<std::string>> romFiles;

    if (std::ifstream file(configFilePath); file.good()) {
        nlohmann::json json;
        try {
            json = nlohmann::json::parse(file);
        } catch (const nlohmann::json::parse_error& error) {
            // TODO: Better warning
            std::cerr << "Unable to parse the config, please make sure it contains valid JSON";
            return 1;
        }
        file.close();
        std::cout << json << std::endl;
        if (json["directories"] != nullptr) {
            for (const auto& directory : json["directories"]) {
                if (std::ifstream file(directory.get<std::string>()); !file.good()) {
                    std::cerr << "Unable to find directory " << directory << std::endl;
                    continue;
                }
                romDirectories.emplace_back(directory.get<std::string>());

                for (const auto& romFile: std::filesystem::directory_iterator(directory.get<std::string>())) {
                    if (romFile.is_directory() || romFile.file_size() > 3584) {
                        continue;
                    }

                    std::cout << "Processing file: " << romFile.path().c_str() << std::endl;

                    // Check if the rom directory doesn't exist in romFiles, then add it
                    if (romFiles.find(&romDirectories.back()) == romFiles.end()) {
                        romFiles.emplace(&romDirectories.back(), std::vector<std::string>());
                    }

                    // Add the file path to the romFiles entry
                    romFiles.find(&romDirectories.back())->second.emplace_back(romFile.path().string());
                }
            }
        }
    }

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL could not initialize. SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    if (!SDL_InitSubSystem(SDL_INIT_AUDIO)) {
        std::cerr << "SDL could not initialize audio. SDL_Error: " << SDL_GetError() << std::endl;
    }

    if (!TTF_Init()) {
        SDL_Log("Couldn't initialize TTF: %s\n", SDL_GetError());
        return 1;
    }

    TTF_Font *font = TTF_OpenFont("assets/font.ttf", 22);
    if (!font) {
        SDL_Log("Failed to load font: %s\n", SDL_GetError());
        return 1;
    }

    std::vector<std::unique_ptr<Window>> windows;
    if (rom.empty()) {
        windows.emplace_back(std::make_unique<MainMenu>(font, configFilePath, romFiles, romDirectories, windows))->init();
    } else {
        windows.emplace_back(std::make_unique<Emulator>(rom))->init();
    }

    bool quit = false;
    SDL_Event event;

    Timer fpsTimer;
    Timer capTimer;
    Timer fpsPrintTimer;

    int countedFrames = 0;
    fpsTimer.start();
    fpsPrintTimer.start();

    while (!quit) {
        capTimer.start();
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                quit = true;
            }

            for (const auto& window : windows) {
                window->handleEvent(event);
            }
        }

        // Do not change, this makes multiple windows not crash
        for (int i = 0; i < windows.size(); ++i) {
            if (windows[i]->isDestroyed()) {
                windows.erase(windows.begin() + i);
                continue;
            }
            windows[i]->update();
            windows[i]->render();
        }

        bool allWindowsClosed = true;
        for (const auto& window : windows) {
            if (window->isShown()){
                allWindowsClosed = false;
                break;
            }
        }

        if (allWindowsClosed || windows.empty()) {
            quit = true;
        }

        float avgFPS = countedFrames / (fpsTimer.getTicks() / 1000.f);
        if (avgFPS > 2000000) {
            avgFPS = 0;
        }

        if (debug && fpsPrintTimer.getTicks() >= 1000) {
            if (!windows.empty()) {
                std::cout << "FPS: " << avgFPS << std::endl;
                for (int i = 0; i < windows.size(); i++) {
                    auto *emulatorPtr = dynamic_cast<Emulator *>(windows[i].get());
                    if (emulatorPtr) {
                        std::cout << "Window " << i << " - " << emulatorPtr->getInstructions() << std::endl;
                        emulatorPtr->resetInstructions();
                    } else {
                         std::cout << "Window " << i << " - Failed to cast Window to Emulator. Might just be the main menu or settings window" << std::endl;
                    }
                }
            }
            fpsPrintTimer.start();
        }

        ++countedFrames;
        int frameTicks = capTimer.getTicks();
        if (frameTicks < 1000 / 60) {
            SDL_Delay(1000 / 60 - frameTicks);
        }
    }

    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();

    return 0;
}