#ifndef INC_8CHOCCHIP_SDLRENDERER_H
#define INC_8CHOCCHIP_SDLRENDERER_H

#include "../../emulator/Renderer.h"
#include <SDL3/SDL.h>

class SdlRenderer : public Renderer {
private:
    const float scale = 15;
public:
    void drawPixel(SDL_Renderer* renderer, uint16_t x, uint16_t y) override;
    float getScale() const;
};

#endif