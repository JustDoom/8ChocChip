#include <SFML/Graphics.hpp>
#include <filesystem>
#include <iostream>
#include <thread>

#include "sfml/Emulator.h"
#include "fstream"

#include "util/MiscUtil.h"

#include "sfml/MainMenu.h"

#include "libconfig.hh"

int main(int argc, char **argv) {
    std::string rom;

    for (int i = 0; i < argc; i++) {
        std::string_view arg = argv[i];
        if (arg.rfind("--") != 0) continue; // TODO: Account for --longform or -sf (short form) commands. just needs a better command handler

        std::string command = MiscUtil::toLowerCase(arg);
        if (command == "--rom") {
            if (i + 1 < argc) {
                return Emulator().launch(argv[++i]);
            } else {
                std::cerr << "Please include the path to the file" << std::endl;
                return 0;
            }
        } else if (command == "--help") {
            std::cerr << "Usage: 8chocchip --rom <rompath>" << std::endl;
            return 0;
        }
    }

    std::string configFilePath = (std::filesystem::path(std::filesystem::path(getenv("HOME")).string()) / ".8chocchip.cfg").string();

    std::vector<std::unique_ptr<std::thread>> windows;

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
                romFiles.find(&romDirectories.back())->second.emplace_back(romFile.path());
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

    MainMenu menu(romFiles, romDirectories, windows, configFilePath);

    return 0;
}