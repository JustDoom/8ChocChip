#include "MainMenu.h"

#include <filesystem>

#include "ui/TextButton.h"

#include "Emulator.h"
#include "../util/MiscUtil.h"

#include "nfd.h"
#include "libconfig.h"
#include "libconfig.hh"

MainMenu::MainMenu(std::unordered_map<std::string *, std::vector<std::string>>& romFiles,
                   std::vector<std::string>& romDirectories, std::vector<std::thread *>& windows,
                   std::string configFilePath) :
    romDirectories(romDirectories), romFiles(romFiles), windows(windows),
    window(sf::VideoMode(640, 480), "8ChocChip - Chip8 Emulator") {

    sf::Vector2u originalWindowSize = this->window.getSize();
    sf::Image icon;
    icon.loadFromFile("../../assets/icon.png");
    this->window.setIcon(64, 64, icon.getPixelsPtr());

    sf::Font font;
    font.loadFromFile("../../assets/font.ttf");

    std::unordered_map<std::string, TextButton> roms;

    for (auto& thing : romFiles) {
        for (std::string& file : thing.second) {

            TextButton romButton(0, 25 * roms.size(), window.getSize().x, 25, MiscUtil::getFileFromPath(file), &font);

            roms.emplace(file, romButton);
        }
    }

    TextButton button(0, 400, 640, 80, "Select ROM", &font);

    while (this->window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::Resized) {
                sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                this->window.setView(sf::View(visibleArea));

                for (auto& romButton : roms) {
                    romButton.second.updateSize(originalWindowSize, this->window.getSize());
                    romButton.second.update(this->window);
                }

                button.updateSize(originalWindowSize, this->window.getSize());
                button.update(this->window);
            }
        }

        for (auto& romButton : roms) {
            romButton.second.update(window);
        }
        button.update(this->window);

        for (auto& romButton : roms) {
            if (!romButton.second.isJustClicked()) continue;

            Emulator emulator;
            std::thread newWindow(&Emulator::launch, &emulator, romButton.first);
            newWindow.detach();
            windows.emplace_back(&newWindow);
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

                    TextButton romButton(0, 25 * roms.size(), this->window.getSize().x, 25, file.path().filename().string(), &font);
                    roms.emplace(file.path().string(), romButton);
                }
                ;
                config.writeFile(configFilePath);
            }
        }

        this->window.clear(sf::Color::White);
        for (auto& romButton : roms) {
            romButton.second.draw(this->window);
        }
        button.draw(this->window);

        this->window.display();
    }
}