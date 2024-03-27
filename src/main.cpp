#include <filesystem>
#include <SFML/Graphics.hpp>
#include <iostream>

#include "Cpu.h"
#include "Keyboard.h"
#include "Renderer.h"
#include "Speaker.h"
#include "fstream"


class TextButton {
private:
    sf::RectangleShape button;
    sf::Text text;
    sf::Font font;
    sf::Color idleColor;
    sf::Color hoverColor;
    sf::Color activeColor;
    bool isPressed;
    bool isHovered;

public:
    TextButton(float x, float y, float width, float height, std::string buttonText) {
        button.setSize(sf::Vector2f(width, height));
        button.setPosition(sf::Vector2f(x, y));

        font.loadFromFile("../assets/font.ttf"); // Load your font here
        text.setFont(font);
        text.setString(buttonText);
        text.setCharacterSize(24); // Set your text size
        text.setFillColor(sf::Color::Black);
        text.setPosition(x + width / 4, y + height / 4); // Adjust text position

        idleColor = sf::Color(192, 192, 192); // Grey
        hoverColor = sf::Color(128, 128, 128); // Dark Grey
        activeColor = sf::Color(64, 64, 64); // Darker Grey

        button.setFillColor(idleColor);

        isPressed = false;
        isHovered = false;
    }

    void updateSize(float width, float height) {
        button.setSize(sf::Vector2f(width, height));
    }

    void update(sf::RenderWindow& window) {
        isHovered = button.getGlobalBounds().contains(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);

        if (isHovered && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            isPressed = true;
            button.setFillColor(activeColor);
        } else if (isHovered) {
            button.setFillColor(hoverColor);
        } else {
            isPressed = false;
            button.setFillColor(idleColor);
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(button);
        window.draw(text);
    }

    bool isClicked() {
        return isPressed;
    }
};

int main(int argc, char** argv)
{

    // sf::RenderWindow window(sf::VideoMode(640, 480), "8ChocChip - Chip8 Emulator");
    //
     TextButton button(300, 200, 200, 80, "CLick!");
    //
    // while (window.isOpen()) {
    //     sf::Event event;
    //     while (window.pollEvent(event)) {
    //         if (event.type == sf::Event::Closed)
    //             window.close();
    //
    //         if (event.type == sf::Event::Resized) {
    //             sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
    //             window.setView(sf::View(visibleArea));
    //             button.updateSize(event.size.width / 4, event.size.height / 3); // Update button size based on window size
    //             button.update(window); // Update button after resizing
    //         }
    //     }
    //
    //     button.update(window);
    //
    //     window.clear(sf::Color::White);
    //     button.draw(window);
    //
    //     window.display();
    // }

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <ROM file>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1], std::ios::binary | std::ios::ate);
    if (file.good()) {
        std::cout << argv[1] << std::endl;
    } else {
        std::cerr << "Can not find file " << argv[1] << std::endl;
        return 1;
    }

    Renderer renderer;
    Speaker speaker;
    Keyboard keyboard;
    Cpu cpu(&renderer, &keyboard, &speaker);

    cpu.loadSpritesIntoMemory();

    cpu.loadProgramIntoMemory(&file);

    sf::RenderWindow window(sf::VideoMode(renderer.getColumns() * renderer.getScale(), renderer.getRows() * renderer.getScale()), "8ChocChip - CHIP-8 Emulator", sf::Style::Titlebar | sf::Style::Close);
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
