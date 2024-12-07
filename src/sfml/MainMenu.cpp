#include "MainMenu.h"

#include <filesystem>
#include <iostream>

#include "libconfig.h"
#include "libconfig.hh"
#include "nfd.h"

#include "Emulator.h"
#include "ui/TextButton.h"
#include "../util/MiscUtil.h"

MainMenu::MainMenu(std::unordered_map<std::string *, std::vector<std::string>>& romFiles,
                   std::vector<std::string>& romDirectories, std::vector<std::unique_ptr<std::thread>>& windows,
                   std::string configFilePath) :
    romDirectories(romDirectories), romFiles(romFiles), windows(windows),
    window(sf::VideoMode(640, 480), "8ChocChip - Chip8 Emulator"),
    inputHandler() {

    sf::Vector2u originalWindowSize = this->window.getSize();
    sf::Image icon;
    icon.loadFromFile("../../assets/icon.png");
    this->window.setIcon(64, 64, icon.getPixelsPtr());
//    this->window.setVerticalSyncEnabled(true);

    sf::Font font;
    font.loadFromFile("../../assets/font.ttf");

    std::unordered_map<std::string, TextButton> roms;

    for (auto& thing : romFiles) {
        for (std::string& file : thing.second) {

            TextButton romButton(0, 25 * roms.size(), this->window.getSize().x, 25, MiscUtil::getFileFromPath(file), &font);

            roms.emplace(file, romButton);
        }
    }

    TextButton button(0, 400, 640, 80, "Select ROM", &font);

    bool focus;
    bool debug = false;

    auto start = std::chrono::high_resolution_clock::now();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_seconds;

    // Variables to count frames
    int frames = 0;
    double fps;

    while (this->window.isOpen()) {
        sf::Event event;
        sf::Vector2i pos = sf::Mouse::getPosition(this->window);

        this->inputHandler.updateLastKeys();
        this->inputHandler.updateLastMouse();

        while (this->window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                this->window.close();
            } else if (event.type == sf::Event::LostFocus) {
                focus = false;
            } else if (event.type == sf::Event::GainedFocus) {
                focus = true;
            } else if (event.type == sf::Event::Resized) {
                sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                this->window.setView(sf::View(visibleArea));

                for (auto& romButton : roms) {
                    romButton.second.updateSize(originalWindowSize, this->window.getSize());
                    romButton.second.update(&this->inputHandler, pos);
                }

                button.updateSize(originalWindowSize, this->window.getSize());
                button.update(&this->inputHandler, pos);
            } else if (event.type == sf::Event::KeyPressed) {
                this->inputHandler.addKey(event.key.code);
            } else if (event.type == sf::Event::KeyReleased) {
                this->inputHandler.removeKey(event.key.code);
            } else if (event.type == sf::Event::MouseButtonPressed) {
                this->inputHandler.addButton(event.mouseButton.button);
            } else if (event.type == sf::Event::MouseButtonReleased) {
                this->inputHandler.removeButton(event.mouseButton.button);
            }
        }

        if (focus) {
            if (this->inputHandler.isJustPressed(sf::Keyboard::F3)) {
                debug = !debug;
            }

            for (auto& romButton : roms) {
                romButton.second.update(&this->inputHandler, pos);

                if (!romButton.second.isJustClicked()) continue;

                Emulator emulator;
                std::thread newWindow(&Emulator::launch, &emulator, romButton.first);
                newWindow.detach();
                windows.emplace_back(&newWindow);
            }
            button.update(&this->inputHandler, pos);

            if (button.isJustClicked()) {
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
                        romFiles.find(&romDirectories.back())->second.emplace_back(file.path().string());

                        TextButton romButton(0, 25 * roms.size(), this->window.getSize().x, 25, file.path().filename().string(), &font);
                        roms.emplace(file.path().string(), romButton);
                    }
                    config.writeFile(configFilePath);
                }
            }
        }

        this->window.clear(sf::Color::White);
        for (auto& romButton : roms) {
            romButton.second.draw(this->window);
        }
        button.draw(this->window);

        this->window.display();

        frames++;

        end = std::chrono::high_resolution_clock::now();
        elapsed_seconds = end - start;

        // If elapsed time is greater than or equal to 1 second
        if (debug && elapsed_seconds.count() >= 1.0) {
            // Calculate FPS
            fps = frames / elapsed_seconds.count();

            // Output FPS
            std::cout << "FPS: " << fps << std::endl;

            // Reset frame count and start time
            frames = 0;
            start = std::chrono::high_resolution_clock::now();
        }
    }
}
