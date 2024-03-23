#include <filesystem>
#include <SFML/Graphics.hpp>
#include <iostream>

#include "Cpu.h"
#include "Keyboard.h"
#include "Renderer.h"
#include "Speaker.h"
#include "fstream"

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <ROM file>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1], std::ios::binary | std::ios::ate);
    if (file.good()) {
        std::cout << argv[1] << std::endl;
    } else {
        std::cerr << "Can not find file" << std::endl;
        return 1;
    }

    Renderer renderer;
    Speaker speaker;
    Keyboard keyboard;
    Cpu cpu(&renderer, &keyboard, &speaker);

    cpu.loadSpritesIntoMemory();

    cpu.loadProgramIntoMemory(&file);

    sf::RenderWindow window(sf::VideoMode(renderer.getColumns() * renderer.getScale(), renderer.getRows() * renderer.getScale()), "CHIP-8 Emulator", sf::Style::Titlebar | sf::Style::Close);
    // TODO: Add icon - window.setIcon();

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