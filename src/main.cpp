#include <SFML/Graphics.hpp>
#include <filesystem>
#include <future>
#include <iostream>

#include "emulator/Cpu.h"
#include "emulator/Keyboard.h"
#include "emulator/Renderer.h"
#include "emulator/Speaker.h"
#include "fstream"

#include "ui/TextButton.h"
#include "util/MiscUtil.h"

#include "nfd.h"

int launch(const std::string& rom, const std::string& executable) {
    if (rom.empty()) {
        std::cerr << "Usage: " << executable << " <ROM file>" << std::endl;
        return 1;
    }

    std::ifstream file(rom, std::ios::binary | std::ios::ate);
    if (file.good()) {
        std::cout << rom << std::endl;
    } else {
        std::cerr << "Can not find file " << rom << std::endl;
        return 1;
    }

    Renderer renderer;
    Speaker speaker;
    Keyboard keyboard;
    Cpu cpu(&renderer, &keyboard, &speaker);

    cpu.loadSpritesIntoMemory();

    cpu.loadProgramIntoMemory(&file);

    sf::RenderWindow window(sf::VideoMode(renderer.getColumns() * renderer.getScale(), renderer.getRows() *
    renderer.getScale()), "8ChocChip - CHIP-8 Emulator", sf::Style::Titlebar | sf::Style::Close);
    sf::Image icon;
    icon.loadFromFile("../assets/icon.png");
    window.setIcon(64, 64, icon.getPixelsPtr());

    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // Handle keyboard inputs
            keyboard.handleEvent(event);
        }

        // Run a cycle of the emulator
        cpu.cycle();

        // Clear the window
        window.clear(sf::Color::Black);

        // Render the window
        renderer.render(&window);
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

    if (quickRom) {
        return launch(rom, argv[0]);
    } else {
        sf::RenderWindow window(sf::VideoMode(640, 480), "8ChocChip - Chip8 Emulator");
        sf::Image icon;
        icon.loadFromFile("../assets/icon.png");
        window.setIcon(64, 64, icon.getPixelsPtr());

        TextButton button(300, 200, 200, 80, "Select ROM");

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();

                if (event.type == sf::Event::Resized) {
                    sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                    window.setView(sf::View(visibleArea));
                    button.updateSize(event.size.width / 4,
                                      event.size.height / 3); // Update button size based on window size
                    button.update(window); // Update button after resizing
                }
            }

            button.update(window);

            if (button.isClicked()) {
                nfdchar_t *outPath = nullptr;
                nfdresult_t result = NFD_OpenDialog(nullptr, nullptr, &outPath);

                if (result == NFD_OKAY) {
                    return launch(outPath, argv[0]);
                    // free(outPath);
                } else if (result == NFD_CANCEL) {
                    puts("User pressed cancel.");
                } else {
                    printf("Error: %s\n", NFD_GetError());
                }
            }

            window.clear(sf::Color::White);
            button.draw(window);

            window.display();
        }
    }

    return 0;
}