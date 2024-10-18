#include <SFML/Graphics.hpp>
#include <filesystem>
#include <iostream>
#include <thread>

#include "sfml/Emulator.h"
#include "fstream"

#include "util/MiscUtil.h"

#include "sfml/MainMenu.h"

#include "libconfig.h"
#include "libconfig.hh"

int main(int argc, char **argv) {
    bool quickRom = false;
    std::string rom;

    for (int i = 0; i < argc; i++) {
        std::string_view arg = argv[i];
        if (arg.rfind("--") != 0) continue; // TODO: Account for --longform or -sf (short form) commands. just needs a better command handler

        std::string command = MiscUtil::toLowerCase(arg);
        if (command == "--rom") {
            if (argc - 1 <= i) {
                std::cerr << "Please include the path to the file" << std::endl;
            } else {
                quickRom = true;
                rom = argv[i + 1];
            }
        }
    }

    std::string home = std::filesystem::path(getenv("HOME")).string();
    std::string configFilePath = (std::filesystem::path(home) / ".8chocchip.cfg").string();

    std::vector<std::thread*> windows;

    std::vector<std::string> romDirectories;
    std::unordered_map<std::string*, std::vector<std::string>> romFiles;

    if (std::ifstream file(configFilePath, std::ios::binary | std::ios::ate); file.good()) {
        libconfig::Config config;
        config.readFile(configFilePath);

        libconfig::Setting &settings = config.getRoot();

        if (!settings.exists("directories")) {
            settings.add("directories", libconfig::Setting::TypeArray);
        }

        libconfig::Setting &directories = settings["directories"];
        romDirectories.reserve(directories.getLength());
        for (int i = 0; i < directories.getLength(); i++) {
            libconfig::Setting &string = directories[i];
            std::string directoryPath = string.c_str();

            romDirectories.emplace_back(directoryPath);

            for (const auto& romFile: std::filesystem::directory_iterator(directoryPath)) {
                if (romFile.is_directory())
                    continue; // Skip directories

                printf("Processing file: %s\n", romFile.path().c_str());

                // Check if the rom directory doesn't exist in romFiles, then add it
                if (romFiles.find(&romDirectories.back()) == romFiles.end()) {
                    romFiles.emplace(&romDirectories.back(), std::vector<std::string>());
                }

                // Add the file path to the romFiles entry
                romFiles.find(&romDirectories.back())->second.emplace_back(romFile.path());
            }
        }
    } else {
        config_t cfg;
        config_init(&cfg);

        config_setting_t *root = config_root_setting(&cfg);
        config_setting_t *list = config_setting_add(root, "directories", CONFIG_TYPE_LIST);

        if (config_write_file(&cfg, configFilePath.c_str()) == CONFIG_FALSE) {
            std::cerr << "Error creating configuration file." << std::endl;
            config_destroy(&cfg);
            return 1;
        }

        std::cout << "Configuration file created successfully." << std::endl;

        config_destroy(&cfg);
    }

    if (quickRom) {
        Emulator emulator;
        return emulator.launch(rom);
    } else {
        MainMenu(romFiles, romDirectories, windows, configFilePath);
    }

    return 0;
}