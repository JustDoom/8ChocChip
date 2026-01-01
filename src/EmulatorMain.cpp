#include "EmulatorMain.h"

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

bool EmulatorMain::initialise(int argc, char **argv) {
    for (int i = 0; i < argc; i++) {
        std::string command = toLowerCase(argv[i]);
        if (command.rfind("--") != 0 && command.rfind('-') != 0) {
            continue;
        }

        if (command == "--path" || command == "-p") {
            if (i + 1 < argc) {
                this->rom = argv[++i];
                continue;
            }

            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Please include the path to the file");
            return false;
        }
        if (command == "--help" || command == "-h") {
            SDL_Log("Usage: 8ChocChip --path <pathtorom/pathtostate>");
            return false;
        }
        if (command == "--debug" || command == "-d") {
            debug = true;
        }
        if (command == "--speedtest" || command == "-st") {
            SDL_Log("Speed");
            speedTest = true;
        }
        if (command == "--instructions" || command == "-ipf") {
            std::cout << argv[++i] << std::endl;
            ipf = std::stoi(argv[i]);
        }
    }

    // TODO: Move config stuff to its own file
    if (!home) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%s environment variable not set.", home);
        return false;
    }
    configFilePath = (std::filesystem::path(home) / ".8chocchip.json").string();

    std::unordered_map<uint8_t, unsigned char> keymap = defaultKeymap;
    if (std::ifstream file(configFilePath); file.good()) {
        nlohmann::json json;
        try {
            json = nlohmann::json::parse(file);
        } catch (const nlohmann::json::parse_error& error) {
            // TODO: Better warning
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Unable to parse the config, please make sure it contains valid JSON");
            return false;
        }
        file.close();
        std::cout << json << std::endl;
        if (json["directories"] != nullptr) {
            for (const auto& directory : json["directories"]) {
                if (std::ifstream file(directory.get<std::string>()); !file.good()) {
                    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Unable to find directory %s", directory.dump().c_str());
                    continue;
                }
                this->romDirectories.emplace_back(directory.get<std::string>());

                searchDirectory(directory.get<std::string>(), this->romFiles, this->romDirectories);
            }
        }
        if (json["keymap"] != nullptr) {
            keymap = json["keymap"];
        }
    }

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL could not initialize. SDL_Error: %s", SDL_GetError());
        return false;
    }

    if (!SDL_InitSubSystem(SDL_INIT_AUDIO)) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL could not initialize audio. SDL_Error: %s", SDL_GetError());
    }

    if (!TTF_Init()) {
        SDL_Log("Couldn't initialize TTF: %s\n", SDL_GetError());
        return false;
    }

    this->font = TTF_OpenFont("assets/font.ttf", 22);
    if (!this->font) {
        SDL_Log("Failed to load font: %s\n", SDL_GetError());
        return false;
    }

    if (this->rom.empty()) {
        this->windows.emplace_back(std::make_unique<MainMenu>(this->font, this->romFiles, this->romDirectories, this->windows))->init();
    } else {
        // FIXME maybe load from file or something like that
        this->windows.emplace_back(std::make_unique<Emulator>(this->rom, RomSettings{}, defaultKeymap))->init(); // TODO: Handle Rom Settings
    }

    this->fpsTimer.start();
    this->fpsPrintTimer.start();

    return true;
}

void EmulatorMain::event(SDL_Event* event) {
    if (event->type == SDL_EVENT_QUIT) {
        this->running = false;
    }

    for (const auto& window : this->windows) {
        window->handleEvent(event);
    }
}

void EmulatorMain::run() {
    this->capTimer.start();

    // Do not change, this makes multiple windows not crash
    for (int i = 0; i < this->windows.size(); ++i) {
        if (this->windows[i]->isDestroyed()) {
            this->windows.erase(this->windows.begin() + i);
            continue;
        }
        this->windows[i]->update();
        this->windows[i]->render();
    }

    bool allWindowsClosed = true;
    for (const auto& window : this->windows) {
        if (window->isShown()){
            allWindowsClosed = false;
            break;
        }
    }

    if (allWindowsClosed || this->windows.empty()) {
        this->running = false;
    }

    float avgFPS = this->countedFrames / (this->fpsTimer.getTicks() / 1000.f);
    if (avgFPS > 2000000) {
        avgFPS = 0;
    }

    if (debug && this->fpsPrintTimer.getTicks() >= 1000) {
        if (!this->windows.empty()) {
            std::cout << "FPS: " << avgFPS << std::endl;
            for (int i = 0; i < this->windows.size(); i++) {
                if (auto* emulatorPtr = dynamic_cast<Emulator*>(this->windows[i].get())) {
                    SDL_Log("Window %i - %i", i, emulatorPtr->getInstructions());
                    emulatorPtr->resetInstructions();
                } else {
                    SDL_Log("Window %i - Failed to cast Window to Emulator. Might just be the main menu or settings window", i);
                }
            }
        }
        this->fpsPrintTimer.start();
    }

    ++this->countedFrames;
    if (const int frameTicks = this->capTimer.getTicks(); frameTicks < 1000 / 60) {
        SDL_Delay(1000 / 60 - frameTicks);
    }
}

void EmulatorMain::quit() const {
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
}

bool EmulatorMain::isRunning() const {
    return this->running;
}
