// This SDL3 renderer for clay has been modified from the official Clay examples
// https://github.com/nicbarker/clay

#ifndef CLAYSDL3RENDERER_H
#define CLAYSDL3RENDERER_H

#include "../dependencies/clay/clay.h"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

typedef struct {

} Clay_SDL3RendererData;

/* Global for convenience. Even in 4K this is enough for smooth curves (low radius or rect size coupled with
 * no AA or low resolution might make it appear as jagged curves) */
static int NUM_CIRCLE_SEGMENTS = 16;

//all rendering is performed by a single SDL call, avoiding multiple RenderRect + plumbing choice for circles.
void SDL_Clay_RenderFillRoundedRect(SDL_Renderer* renderer, const SDL_FRect rect, const float cornerRadius, const Clay_Color _color);
void SDL_Clay_RenderArc(SDL_Renderer* renderer, const SDL_FPoint center, const float radius, const float startAngle, const float endAngle, const float thickness, const Clay_Color color);
void SDL_Clay_RenderClayCommands(SDL_Renderer* renderer, TTF_TextEngine* textEngine, TTF_Font** fonts, Clay_RenderCommandArray *rcommands);

#endif