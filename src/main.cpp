#include <filesystem>
#include <fstream>
#include <iostream>
#include <unordered_map>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <libconfig.h++>

#include "Timer.h"
#include "sdl/Emulator.h"
#include "sdl/MainMenu.h"
#include "util/Constants.h"
#include "util/MiscUtil.h"

int main(int argc, char **argv) {
    std::string rom;

    for (int i = 0; i < argc; i++) {
        std::string_view arg = argv[i];
        if (arg.rfind("--") != 0) {
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
    }

    if (!home) {
        std::cerr << home << " environment variable not set. " << std::endl;
        return 1;
    }

    std::string configFilePath = (std::filesystem::path(home) / ".8chocchip.cfg").string();

    std::vector<std::string> romDirectories;
    std::unordered_map<std::string*, std::vector<std::string>> romFiles;

    libconfig::Config config;
    if (std::ifstream file(configFilePath, std::ios::binary | std::ios::ate); file.good()) {
        config.readFile(configFilePath);

        libconfig::Setting &settings = config.getRoot();
        if (!settings.exists("directories")) {
            settings.add("directories", libconfig::Setting::TypeArray);
        }

        libconfig::Setting& directories = settings["directories"];
        romDirectories.reserve(directories.getLength());
        for (int i = 0; i < directories.getLength(); i++) {
            libconfig::Setting &string = directories[i];
            std::string directoryPath = string.c_str();

            romDirectories.emplace_back(directoryPath);

            for (const auto& romFile: std::filesystem::directory_iterator(directoryPath)) {
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
    } else {
        try {
            config.getRoot().add("directories", libconfig::Setting::TypeList);
            config.writeFile(configFilePath.c_str());

            std::cout << "Configuration file created successfully." << std::endl;
        } catch (const libconfig::FileIOException &ioException) {
            std::cerr << "I/O error while writing the configuration file." << std::endl;
            return 1;
        } catch (const libconfig::SettingException &settingException) {
            std::cerr << "Setting error: " << settingException.what() << std::endl;
            return 1;
        }
    }

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
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

    bool debug = true;
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
            if(window->isShown()){
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

        if (debug) {
            if (fpsPrintTimer.getTicks() >= 1000) {
                if (!windows.empty()) {
                    auto* emulatorPtr = dynamic_cast<Emulator*>(windows[1].get());
                    if (emulatorPtr) {
                        std::cout << "FPS: " << avgFPS << " - " << emulatorPtr->getInstructions() << std::endl;
                        emulatorPtr->resetInstructions();
                    } else {
                        std::cout << "FPS: " << avgFPS << " - Failed to cast Window to Emulator\n";
                    }
                }
                fpsPrintTimer.start();
            }
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