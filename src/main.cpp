#include <filesystem>
#include <iostream>
#include <thread>
#include <unordered_map>
#include "sdl/MainMenu.h"

#include <fstream>

#include "util/MiscUtil.h"

#include "libconfig.hh"

#include <SDL3_ttf/SDL_ttf.h>

int main(int argc, char **argv) {
    std::string rom;

    for (int i = 0; i < argc; i++) {
        std::string_view arg = argv[i];
        if (arg.rfind("--") != 0) continue; // TODO: Account for --longform or -sf (short form) commands. just needs a better command handler

        std::string command = MiscUtil::toLowerCase(std::string(arg));
        if (command == "--rom") {
            if (i + 1 < argc) {
                // return Emulator().launch(argv[++i]);
            }
            std::cerr << "Please include the path to the file" << std::endl;
            return 0;
        }
        if (command == "--help") {
            std::cerr << "Usage: 8chocchip --rom <rompath>" << std::endl;
            return 0;
        }
    }

    const char* home = nullptr;
#ifdef _WIN32
    home = std::getenv("USERPROFILE");
#else
    home = std::getenv("HOME");
#endif
    if (!home) {
        std::cerr << "HOME environment variable not set!" << std::endl;
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
                if (romFile.is_directory()) {
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

    if (!TTF_Init()) {
        SDL_Log("Couldn't initialize TTF: %s\n",SDL_GetError());
        return 1;
    }

    TTF_Font *font = TTF_OpenFont("assets/font.ttf", 22);
    if (!font) {
        SDL_Log("Failed to load font: %s\n", SDL_GetError());
        return 1;
    }

    std::vector<std::unique_ptr<Window>> windows;
    windows.emplace_back(std::make_unique<MainMenu>(font, configFilePath, romFiles, romDirectories, windows))->init();

    bool quit = false;
    SDL_Event event;

    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                quit = true;
            }

            for (const auto& window : windows) {
                window->handleEvent(event);
            }
        }

        // Do not change, this makes multiple windows not crash
        for (size_t i = 0; i < windows.size(); ++i) {
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

        if (allWindowsClosed) {
            quit = true;
        }
    }

    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();

    return 0;
}