#ifndef INC_8CHOCCHIP_SFMLRENDERER_H
#define INC_8CHOCCHIP_SFMLRENDERER_H

#include <SFML/Graphics.hpp>

#include "../emulator/Renderer.h"

class SfmlRenderer : public Renderer {
private:
    sf::RenderWindow* window;

    uint8_t scale;
public:
    SfmlRenderer(sf::RenderWindow* window);

    void drawPixel(uint16_t x, uint16_t y) override;
    uint8_t getScale() const;
};

#endif // INC_8CHOCCHIP_SFMLRENDERER_H
