#include "EmulatorEntry.h"

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

// Includes the definitions for clay. Include the clay.h file normally without definition for normal use
#define CLAY_IMPLEMENTATION
#include "../dependencies/clay/clay.h"

void EmulatorEntry::start(int argc, char **argv) {
    std::string rom;

    for (int i = 0; i < argc; i++) {
        std::string command = toLowerCase(argv[i]);
        if (command.rfind("--") != 0 && command.rfind('-') != 0) {
            continue;
        }

        if (command == "--path" || command == "-p") {
            if (i + 1 < argc) {
                rom = argv[++i];
                continue;
            }

            std::cerr << "Please include the path to the file" << std::endl;
            return;
        }
        if (command == "--help" || command == "-h") {
            std::cerr << "Usage: 8ChocChip --path <pathtorom/pathtostate>" << std::endl;
            return;
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
        return;
    }
    configFilePath = (std::filesystem::path(home) / ".8chocchip.json").string();

    std::vector<std::string> romDirectories;
    std::unordered_map<std::string*, std::vector<std::string>> romFiles;
    std::unordered_map<std::string*, std::vector<std::string>> stateFiles;

    if (std::ifstream file(configFilePath); file.good()) {
        nlohmann::json json;
        try {
            json = nlohmann::json::parse(file);
        } catch (const nlohmann::json::parse_error& error) {
            // TODO: Better warning
            std::cerr << "Unable to parse the config, please make sure it contains valid JSON";
            return;
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

                searchDirectory(directory.get<std::string>(), romFiles, stateFiles, romDirectories);
            }
        }
    }

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL could not initialize. SDL_Error: " << SDL_GetError() << std::endl;
        return;
    }

    if (!SDL_InitSubSystem(SDL_INIT_AUDIO)) {
        std::cerr << "SDL could not initialize audio. SDL_Error: " << SDL_GetError() << std::endl;
    }

    if (!TTF_Init()) {
        SDL_Log("Couldn't initialize TTF: %s\n", SDL_GetError());
        return;
    }

    TTF_Font *font = TTF_OpenFont("assets/font.ttf", 22);
    if (!font) {
        SDL_Log("Failed to load font: %s\n", SDL_GetError());
        return;
    }

    std::vector<std::unique_ptr<Window>> windows;
    if (rom.empty()) {
        windows.emplace_back(std::make_unique<MainMenu>(font, romFiles, stateFiles, romDirectories, windows))->init();
    } else {
        windows.emplace_back(std::make_unique<Emulator>(rom, RomSettings{}))->init(); // TODO: Handle Rom Settings
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
}
