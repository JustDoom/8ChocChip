#include "Emulator.h"

#include "fstream"
#include "iostream"

#include "../emulator/Cpu.h"
#include "emulator/SfmlKeyboard.h"
#include "emulator/SfmlRenderer.h"
#include "emulator/SfmlSpeaker.h"

int Emulator::launch(const std::string &rom) {
    if (rom.empty()) {
        std::cerr << "No ROM file has been specified :(" << std::endl;
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
