#ifndef INC_8CHOCCHIP_SDLRENDERER_H
#define INC_8CHOCCHIP_SDLRENDERER_H

#include "../../emulator/Renderer.h"
#include "SDL2/SDL.h"

class SdlRenderer : public Renderer {
private:
    SDL_Renderer* window;

    uint8_t scale;
public:
    SdlRenderer(SDL_Renderer* window);

    void drawPixel(uint16_t x, uint16_t y) override;
    uint8_t getScale() const;
};

#endif // INC_8CHOCCHIP_SDLRENDERER_H
