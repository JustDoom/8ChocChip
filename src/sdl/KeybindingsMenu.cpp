#include "KeybindingsMenu.h"

#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "../ClaySDL3Renderer.h"
#include "../util/MiscUtil.h"
#include "../util/Constants.h"

KeybindingsMenu::KeybindingsMenu(TTF_Font* font) {
    this->fonts = (TTF_Font**) SDL_calloc(1, sizeof(TTF_Font *));
    this->fonts[0] = font;

    this->width = 200;
    this->height = 200;
}

void KeybindingsMenu::init() {
    Window::init();

    this->textEngine = TTF_CreateRendererTextEngine(this->renderer);

    uint64_t totalMemorySize = Clay_MinMemorySize();
    Clay_Arena clayMemory = (Clay_Arena) {
        .capacity = totalMemorySize,
        .memory = (char*) SDL_malloc(totalMemorySize)
    };

    int width, height;
    SDL_GetWindowSize(this->window, &width, &height);
    Clay_Initialize(clayMemory, (Clay_Dimensions) { (float) width, (float) height }, (Clay_ErrorHandler) { handleClayErrors });
    Clay_SetMeasureTextFunction(SDL_MeasureText, this->fonts);
}

bool KeybindingsMenu::handleEvent(SDL_Event& event) {
    if (!Window::handleEvent(event)) {
        return false;
    }

    if (event.window.windowID == this->windowId) {        
        // TODO manage events
    }

    return true;
}

void KeybindingsMenu::update() {

}

void KeybindingsMenu::render() {
    Clay_SetLayoutDimensions((Clay_Dimensions) { (float) this->width, (float) this->height });
    Clay_BeginLayout();
    
    CLAY({ .id = CLAY_ID("KeybindsConfiguration"), .layout = { .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(8), .childGap = 8, .layoutDirection = CLAY_TOP_TO_BOTTOM }, .backgroundColor = {250, 250, 250, 250} }) {
        CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .childGap = 8} }) {
            CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(8), .childGap = 8, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } }, .backgroundColor = {90, 60, 40, 255} }) {
                CLAY_TEXT(CLAY_STRING("1"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                Clay_OnHover(nullptr, reinterpret_cast<intptr_t>(new std::string("1")));
            }
            CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(8), .childGap = 8, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } }, .backgroundColor = {90, 60, 40, 255} }) {
                CLAY_TEXT(CLAY_STRING("2"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                Clay_OnHover(nullptr, reinterpret_cast<intptr_t>(new std::string("2")));
            }
            CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(8), .childGap = 8, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } }, .backgroundColor = {90, 60, 40, 255} }) {
                CLAY_TEXT(CLAY_STRING("3"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                Clay_OnHover(nullptr, reinterpret_cast<intptr_t>(new std::string("3")));
            }
            CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(8), .childGap = 8, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } }, .backgroundColor = {90, 60, 40, 255} }) {
                CLAY_TEXT(CLAY_STRING("C"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                Clay_OnHover(nullptr, reinterpret_cast<intptr_t>(new std::string("C")));
            }
        }
        CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .childGap = 8} }) {
            CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(8), .childGap = 8, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } }, .backgroundColor = {90, 60, 40, 255} }) {
                CLAY_TEXT(CLAY_STRING("4"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                Clay_OnHover(nullptr, reinterpret_cast<intptr_t>(new std::string("4")));
            }
            CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(8), .childGap = 8, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } }, .backgroundColor = {90, 60, 40, 255} }) {
                CLAY_TEXT(CLAY_STRING("5"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                Clay_OnHover(nullptr, reinterpret_cast<intptr_t>(new std::string("5")));
            }
            CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(8), .childGap = 8, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } }, .backgroundColor = {90, 60, 40, 255} }) {
                CLAY_TEXT(CLAY_STRING("6"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                Clay_OnHover(nullptr, reinterpret_cast<intptr_t>(new std::string("6")));
            }
            CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(8), .childGap = 8, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } }, .backgroundColor = {90, 60, 40, 255} }) {
                CLAY_TEXT(CLAY_STRING("D"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                Clay_OnHover(nullptr, reinterpret_cast<intptr_t>(new std::string("D")));
            }
        }
        CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .childGap = 8} }) {
            CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(8), .childGap = 8, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } }, .backgroundColor = {90, 60, 40, 255} }) {
                CLAY_TEXT(CLAY_STRING("7"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                Clay_OnHover(nullptr, reinterpret_cast<intptr_t>(new std::string("7")));
            }
            CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(8), .childGap = 8, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } }, .backgroundColor = {90, 60, 40, 255} }) {
                CLAY_TEXT(CLAY_STRING("8"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                Clay_OnHover(nullptr, reinterpret_cast<intptr_t>(new std::string("8")));
            }
            CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(8), .childGap = 8, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } }, .backgroundColor = {90, 60, 40, 255} }) {
                CLAY_TEXT(CLAY_STRING("9"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                Clay_OnHover(nullptr, reinterpret_cast<intptr_t>(new std::string("9")));
            }
            CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(8), .childGap = 8, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } }, .backgroundColor = {90, 60, 40, 255} }) {
                CLAY_TEXT(CLAY_STRING("E"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                Clay_OnHover(nullptr, reinterpret_cast<intptr_t>(new std::string("E")));
            }
        }
        CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .childGap = 8} }) {
            CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(8), .childGap = 8, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } }, .backgroundColor = {90, 60, 40, 255} }) {
                CLAY_TEXT(CLAY_STRING("A"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                Clay_OnHover(nullptr, reinterpret_cast<intptr_t>(new std::string("A")));
            }
            CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(8), .childGap = 8, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } }, .backgroundColor = {90, 60, 40, 255} }) {
                CLAY_TEXT(CLAY_STRING("0"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                Clay_OnHover(nullptr, reinterpret_cast<intptr_t>(new std::string("0")));
            }
            CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(8), .childGap = 8, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } }, .backgroundColor = {90, 60, 40, 255} }) {
                CLAY_TEXT(CLAY_STRING("B"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                Clay_OnHover(nullptr, reinterpret_cast<intptr_t>(new std::string("B")));
            }
            CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(8), .childGap = 8, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } }, .backgroundColor = {90, 60, 40, 255} }) {
                CLAY_TEXT(CLAY_STRING("F"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                Clay_OnHover(nullptr, reinterpret_cast<intptr_t>(new std::string("F")));
            }
        }
    }

    Clay_RenderCommandArray renderCommands = Clay_EndLayout();

    SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 255);
    SDL_RenderClear(this->renderer);

    SDL_Clay_RenderClayCommands(this->renderer, this->textEngine, this->fonts, &renderCommands);
    clearClayStringBuffers();

    SDL_RenderPresent(this->renderer);
}

void KeybindingsMenu::resize(SDL_Event& event) {

}

void KeybindingsMenu::close() {
    TTF_DestroyRendererTextEngine(this->textEngine);
    Window::close();
}

