#include <SFML/Graphics.hpp>
#include <filesystem>
#include <future>
#include <iostream>

#include "emulator/Cpu.h"
#include "sfml/SfmlKeyboard.h"
#include "sfml/SfmlRenderer.h"
#include "sfml/SfmlSpeaker.h"
#include "fstream"

#include "sfml/ui/TextButton.h"
#include "util/MiscUtil.h"

#include "nfd.h"
#include "libconfig.h"
#include "libconfig.hh"

int launch(const std::string& rom, const std::string& executable) {
    if (rom.empty()) {
        std::cerr << "Usage: " << executable << " <ROM file>" << std::endl;
        return 1;
    }

    std::ifstream file(rom, std::ios::binary | std::ios::ate);
    if (!file.good()) {
        std::cerr << "Can not find file " << rom << std::endl;
        return 1;
    }

    sf::RenderWindow window(sf::VideoMode(64 * 15, 32 * 15), "8ChocChip - CHIP-8 Emulator", sf::Style::Titlebar | sf::Style::Close);
    sf::Image icon;
    icon.loadFromFile("../assets/icon.png");
    window.setIcon(64, 64, icon.getPixelsPtr());

    SfmlRenderer renderer(&window);

    SfmlSpeaker speaker;
    SfmlKeyboard keyboard;
    Cpu cpu(&renderer, &keyboard, &speaker);

    cpu.loadSpritesIntoMemory();

    cpu.loadProgramIntoMemory(&file);

    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // Handle keyboard inputs
            if (event.type == sf::Event::KeyPressed) {
                keyboard.handleKeyDown(event.key.code);
            } else if (event.type == sf::Event::KeyReleased) {
                keyboard.handleKeyUp(event.key.code);
            }
        }

        // Run a cycle of the emulator
        cpu.cycle();

        // Clear the window
        window.clear(sf::Color::Black);

        // Render the window
        renderer.render();
        window.display();

        // Keep it at 60fps
        sf::Time elapsed = clock.restart();
        sf::Time targetFrameTime = sf::seconds(1.0f / 60.0f);
        if (elapsed < targetFrameTime) {
            sleep(targetFrameTime - elapsed);
        }
    }

     return 0;
}

std::string getFileFromPath(std::string path) {
    size_t lastSlashPos = path.find_last_of('/');

    if (lastSlashPos != std::string::npos) {
        return path.substr(lastSlashPos + 1);
    }

    return path;
}

int main(int argc, char **argv) {
    bool quickRom = false;
    std::string rom;

    for (int i = 0; i < argc; i++) {
        std::string arg = argv[i];
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

    std::vector<std::string> romDirectories;
    std::unordered_map<std::string*, std::vector<std::string>> romFiles;

    std::ifstream file(configFilePath, std::ios::binary | std::ios::ate);
    if (file.good()) {
        libconfig::Config config;
        config.readFile(configFilePath);

        libconfig::Setting &settings = config.getRoot();

        if (!settings.exists("directories")) {
            settings.add("directories", libconfig::Setting::TypeArray);
        }

        libconfig::Setting &directories = settings["directories"];
        for (int i = 0; i < directories.getLength(); i++) {
            libconfig::Setting &string = directories[i];
            std::string directoryPath = string.c_str();

            romDirectories.emplace_back(directoryPath);

            for (const auto& file: std::filesystem::directory_iterator(directoryPath)) {
                if (file.is_directory())
                    continue; // Skip directories

                printf("Processing file: %s\n", file.path().c_str());

                // Check if the rom directory doesn't exist in romFiles, then add it
                if (romFiles.find(&romDirectories.back()) == romFiles.end()) {
                    romFiles.emplace(&romDirectories.back(), std::vector<std::string>());
                }

                // Add the file path to the romFiles entry
                romFiles.find(&romDirectories.back())->second.emplace_back(file.path());
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
        return launch(rom, argv[0]);
    } else {
        sf::RenderWindow window(sf::VideoMode(640, 480), "8ChocChip - Chip8 Emulator");
        sf::Vector2u originalWindowSize = window.getSize();
        sf::Image icon;
        icon.loadFromFile("../assets/icon.png");
        window.setIcon(64, 64, icon.getPixelsPtr());

        sf::Font font;
        font.loadFromFile("../assets/font.ttf");

        std::unordered_map<std::string, TextButton> roms;

        for (auto& thing : romFiles) {
            for (std::string& file : thing.second) {

                TextButton romButton(0, 25 * roms.size(), window.getSize().x, 25, getFileFromPath(file), &font);

                roms.emplace(file, romButton);
            }
        }

        TextButton button(0, 400, 640, 80, "Select ROM", &font);

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }

                if (event.type == sf::Event::Resized) {
                    sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                    window.setView(sf::View(visibleArea));

                    for (auto& romButton : roms) {
                        romButton.second.updateSize(originalWindowSize, window.getSize());
                        romButton.second.update(window);
                    }

                    button.updateSize(originalWindowSize, window.getSize());
                    button.update(window);
                }
            }

            for (auto& romButton : roms) {
                romButton.second.update(window);
            }
            button.update(window);

            for (auto& romButton : roms) {
                if (!romButton.second.isClicked()) continue;

                return launch(romButton.first, argv[0]);
            }
            if (button.isClicked()) {
                nfdchar_t* outPath = nullptr;
                nfdresult_t result = NFD_PickFolder(nullptr, &outPath);

                if (result == NFD_OKAY) {
                    libconfig::Config config;
                    config.readFile(configFilePath);

                    libconfig::Setting& settings = config.getRoot();

                    if (!settings.exists("directories")) {
                        settings.add("directories", libconfig::Setting::TypeArray);
                    }

                    libconfig::Setting& directories = settings["directories"];
                    directories.add(libconfig::Setting::TypeString) = outPath;

                    romDirectories.emplace_back(outPath);

                    for (const auto& file : std::filesystem::directory_iterator(outPath)) {
                        if (file.is_directory()) continue; // TODO: Make sure its a file that can be emulated, at least basic checks so it isn't like a word doc

                        printf("Processing file - : %s\n", file.path().c_str());

                        // Check if the rom directory doesn't exist in romFiles, then add it
                        if (romFiles.find(&romDirectories.back()) == romFiles.end()) {
                            romFiles.emplace(&romDirectories.back(), std::vector<std::string>());
                        }

                        // Add the file path to the romFiles entry
                        romFiles.find(&romDirectories.back())->second.emplace_back(file.path());

                        TextButton romButton(0, 25 * roms.size(), window.getSize().x, 25, file.path().filename().string(), &font);
                        roms.emplace(file.path().string(), romButton);
                    }
;
                    config.writeFile(configFilePath);
                }
            }

            window.clear(sf::Color::White);
            for (auto& romButton : roms) {
                romButton.second.draw(window);
            }
            button.draw(window);

            window.display();
        }
    }

    return 0;
}