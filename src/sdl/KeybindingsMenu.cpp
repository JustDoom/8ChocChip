#include "KeybindingsMenu.h"

#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "../ClaySDL3Renderer.h"
#include "../util/MiscUtil.h"
#include "../util/Constants.h"

bool keybindingsMenuClicked = false;

KeybindingsMenu::KeybindingsMenu(TTF_Font* font, std::unordered_map<uint8_t, unsigned char>* keymap) {
    this->fonts = (TTF_Font**) SDL_calloc(1, sizeof(TTF_Font *));
    this->fonts[0] = font;
    this->keymap = keymap;

    this->width = 200;
    this->height = 250;
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
        switch(event.type) {
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    keybindingsMenuClicked = true;
                }
            break;
            case SDL_EVENT_MOUSE_BUTTON_UP:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    keybindingsMenuClicked = false;
                }
            break;
            case SDL_EVENT_KEY_DOWN:
                if (this->keyWaitingFor) {
                    // The user pressed the new keybind for the chosen key
                    for (auto& item : *keymap) {
                        if (item.second == *this->keyWaitingFor) {
                            keymap->erase(item.first);
                            break;
                        }
                    }
                    unsigned char pressed_key = *this->keyWaitingFor;
                    keymap->insert_or_assign(static_cast<uint8_t>(event.key.scancode), pressed_key);
                    this->keyWaitingFor = nullptr;
                }
            break;
        }
    }

    return true;
}

void KeybindingsMenu::update() {
    if (!this->mouseFocus) {
        keybindingsMenuClicked = false;
        return;
    }
}

void KeybindingsMenu::render() {
    float x,y;
    SDL_GetMouseState(&x, &y);
    Clay_SetLayoutDimensions((Clay_Dimensions) { (float) this->width, (float) this->height });
    Clay_SetPointerState((Clay_Vector2) { x, y }, keybindingsMenuClicked);
    Clay_UpdateScrollContainers(false, (Clay_Vector2) { 0, 0 }, 0.0166f);

    Clay_SetLayoutDimensions((Clay_Dimensions) { (float) this->width, (float) this->height });
    Clay_BeginLayout();
    
    CLAY({ .id = CLAY_ID("KeybindsConfiguration"), .layout = { .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(8), .childGap = 8, .layoutDirection = CLAY_TOP_TO_BOTTOM }, .backgroundColor = {250, 250, 250, 250} }) {
        CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .childGap = 8} }) {
            CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(8), .childGap = 8, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } }, .backgroundColor = {90, 60, 40, 255} }) {
                CLAY_TEXT(CLAY_STRING("1"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                Clay_OnHover(handleKeybindClick, reinterpret_cast<intptr_t>(&this->dataList.emplace_back(this, new uint8_t(0x1))));
            }
            CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(8), .childGap = 8, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } }, .backgroundColor = {90, 60, 40, 255} }) {
                CLAY_TEXT(CLAY_STRING("2"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                Clay_OnHover(handleKeybindClick, reinterpret_cast<intptr_t>(&this->dataList.emplace_back(this, new uint8_t(0x2))));
            }
            CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(8), .childGap = 8, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } }, .backgroundColor = {90, 60, 40, 255} }) {
                CLAY_TEXT(CLAY_STRING("3"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                Clay_OnHover(handleKeybindClick, reinterpret_cast<intptr_t>(&this->dataList.emplace_back(this, new uint8_t(0x3))));
            }
            CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(8), .childGap = 8, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } }, .backgroundColor = {90, 60, 40, 255} }) {
                CLAY_TEXT(CLAY_STRING("C"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                Clay_OnHover(handleKeybindClick, reinterpret_cast<intptr_t>(&this->dataList.emplace_back(this, new uint8_t(0xC))));
            }
        }
        CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .childGap = 8} }) {
            CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(8), .childGap = 8, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } }, .backgroundColor = {90, 60, 40, 255} }) {
                CLAY_TEXT(CLAY_STRING("4"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                Clay_OnHover(handleKeybindClick, reinterpret_cast<intptr_t>(&this->dataList.emplace_back(this, new uint8_t(0x4))));
            }
            CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(8), .childGap = 8, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } }, .backgroundColor = {90, 60, 40, 255} }) {
                CLAY_TEXT(CLAY_STRING("5"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                Clay_OnHover(handleKeybindClick, reinterpret_cast<intptr_t>(&this->dataList.emplace_back(this, new uint8_t(0x5))));
            }
            CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(8), .childGap = 8, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } }, .backgroundColor = {90, 60, 40, 255} }) {
                CLAY_TEXT(CLAY_STRING("6"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                Clay_OnHover(handleKeybindClick, reinterpret_cast<intptr_t>(&this->dataList.emplace_back(this, new uint8_t(0x6))));
            }
            CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(8), .childGap = 8, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } }, .backgroundColor = {90, 60, 40, 255} }) {
                CLAY_TEXT(CLAY_STRING("D"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                Clay_OnHover(handleKeybindClick, reinterpret_cast<intptr_t>(&this->dataList.emplace_back(this, new uint8_t(0xD))));
            }
        }
        CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .childGap = 8} }) {
            CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(8), .childGap = 8, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } }, .backgroundColor = {90, 60, 40, 255} }) {
                CLAY_TEXT(CLAY_STRING("7"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                Clay_OnHover(handleKeybindClick, reinterpret_cast<intptr_t>(&this->dataList.emplace_back(this, new uint8_t(0x7))));
            }
            CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(8), .childGap = 8, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } }, .backgroundColor = {90, 60, 40, 255} }) {
                CLAY_TEXT(CLAY_STRING("8"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                Clay_OnHover(handleKeybindClick, reinterpret_cast<intptr_t>(&this->dataList.emplace_back(this, new uint8_t(0x8))));
            }
            CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(8), .childGap = 8, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } }, .backgroundColor = {90, 60, 40, 255} }) {
                CLAY_TEXT(CLAY_STRING("9"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                Clay_OnHover(handleKeybindClick, reinterpret_cast<intptr_t>(&this->dataList.emplace_back(this, new uint8_t(0x9))));
            }
            CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(8), .childGap = 8, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } }, .backgroundColor = {90, 60, 40, 255} }) {
                CLAY_TEXT(CLAY_STRING("E"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                Clay_OnHover(handleKeybindClick, reinterpret_cast<intptr_t>(&this->dataList.emplace_back(this, new uint8_t(0xE))));
            }
        }
        CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .childGap = 8} }) {
            CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(8), .childGap = 8, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } }, .backgroundColor = {90, 60, 40, 255} }) {
                CLAY_TEXT(CLAY_STRING("A"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                Clay_OnHover(handleKeybindClick, reinterpret_cast<intptr_t>(&this->dataList.emplace_back(this, new uint8_t(0xA))));
            }
            CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(8), .childGap = 8, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } }, .backgroundColor = {90, 60, 40, 255} }) {
                CLAY_TEXT(CLAY_STRING("0"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                Clay_OnHover(handleKeybindClick, reinterpret_cast<intptr_t>(&this->dataList.emplace_back(this, new uint8_t(0x0))));
            }
            CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(8), .childGap = 8, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } }, .backgroundColor = {90, 60, 40, 255} }) {
                CLAY_TEXT(CLAY_STRING("B"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                Clay_OnHover(handleKeybindClick, reinterpret_cast<intptr_t>(&this->dataList.emplace_back(this, new uint8_t(0xB))));
            }
            CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(8), .childGap = 8, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } }, .backgroundColor = {90, 60, 40, 255} }) {
                CLAY_TEXT(CLAY_STRING("F"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                Clay_OnHover(handleKeybindClick, reinterpret_cast<intptr_t>(&this->dataList.emplace_back(this, new uint8_t(0xF))));
            }
        }

        CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(8), .childGap = 8, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } }, .backgroundColor = {90, 60, 40, 255} }) {
            if (this->keyWaitingFor) {
                CLAY_TEXT(toClayString("Press a key"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
            } else {
                CLAY_TEXT(toClayString("Reset"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                Clay_OnHover(handleResetKeybindings, reinterpret_cast<intptr_t>(&this->dataList.emplace_back(this, nullptr)));
            }
        }
    }

    Clay_RenderCommandArray renderCommands = Clay_EndLayout();

    SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 255);
    SDL_RenderClear(this->renderer);

    SDL_Clay_RenderClayCommands(this->renderer, this->textEngine, this->fonts, &renderCommands);
    clearClayStringBuffers();

    SDL_RenderPresent(this->renderer);

    this->dataList.clear();
}

void KeybindingsMenu::resize(SDL_Event& event) {

}

void KeybindingsMenu::close() {
    TTF_DestroyRendererTextEngine(this->textEngine);
    Window::close();
}

void KeybindingsMenu::handleKeybindClick(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData) {
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        const auto data = reinterpret_cast<KeybindingHoverData*>(userData);
        data->self->keyWaitingFor = data->key;
    }
}

void KeybindingsMenu::handleResetKeybindings(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData) {
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        const auto data = reinterpret_cast<KeybindingHoverData*>(userData);
        *data->self->keymap = defaultKeymap;
    }
}

