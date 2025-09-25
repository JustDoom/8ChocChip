#include "MainMenu.h"

#include <filesystem>
#include <fstream>
#include <iostream>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

#include "../ClaySDL3Renderer.h"
#include "../util/Constants.h"
#include "../util/MiscUtil.h"
#include "Emulator.h"
#include "KeybindingsMenu.h"

constexpr auto COLOR_BOX = (Clay_Color) {224, 215, 210, 255};
constexpr auto COLOR_TITLE = (Clay_Color) {140, 100, 60, 255};
constexpr auto COLOR_ROMS = (Clay_Color) {180, 140, 90, 255};
constexpr auto COLOR_HIGHLIGHT = (Clay_Color) {110, 80, 50, 255};
constexpr auto COLOR_BUTTON = (Clay_Color) {90, 60, 40, 255};

bool clicked = false;
Clay_Vector2 wheel{};

MainMenu::MainMenu(TTF_Font* font, std::unordered_map<std::string *, std::vector<std::string>> &romFiles,
                   std::unordered_map<std::string *, std::vector<std::string>> &stateFiles,
                   std::vector<std::string> &romDirectories, std::vector<std::unique_ptr<Window>> &windows) :
    romDirectories(romDirectories), romFiles(romFiles), stateFiles(stateFiles), windows(windows), mutex(SDL_CreateMutex()) {
    this->fonts = (TTF_Font**) SDL_calloc(1, sizeof(TTF_Font *));
    this->fonts[0] = font;
}

void MainMenu::init() {
    Window::init();

    this->renderer = this->renderer;
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

bool MainMenu::handleEvent(SDL_Event &event) {
    if (!Window::handleEvent(event)) {
        return false;
    }

    switch (event.type) {
        case SDL_EVENT_KEY_DOWN:
            this->inputHandler.addKey(event.key.scancode);
        break;
        case SDL_EVENT_KEY_UP:
            this->inputHandler.removeKey(event.key.scancode);
        break;
        case SDL_EVENT_MOUSE_BUTTON_UP:
            this->inputHandler.removeButton(event.button.button);
        if (event.button.button == SDL_BUTTON_LEFT) {
            clicked = false;
        }
        break;
        case SDL_EVENT_MOUSE_MOTION:
            // Clay_SetPointerState((Clay_Vector2) { event.motion.x, event.motion.y }, event.motion.state & SDL_BUTTON_LMASK);
        break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            this->inputHandler.addButton(event.button.button);
            if (event.button.button == SDL_BUTTON_LEFT) {
                clicked = true;
            }
        break;
        case SDL_EVENT_MOUSE_WHEEL:
            wheel.x = event.wheel.x * 2;
            wheel.y = event.wheel.y * 2;
        break;
    }

    return true;
}

void MainMenu::update() {
    if (!this->mouseFocus) {
        clicked = false;
        return;
    }

    if (this->inputHandler.isJustPressed(SDL_SCANCODE_F3)) {
        this->debug = !this->debug;
    }

    this->inputHandler.updateLastKeys();
    this->inputHandler.updateLastMouse();
}

void MainMenu::render() {
    float x, y;
    SDL_GetMouseState(&x, &y);
    Clay_SetLayoutDimensions((Clay_Dimensions) { (float) this->width, (float) this->height });
    Clay_SetPointerState((Clay_Vector2) { x, y }, clicked);
    Clay_UpdateScrollContainers(false, (Clay_Vector2) { wheel.x, wheel.y }, 0.0166f);
    wheel.x = 0;
    wheel.y = 0;

    Clay_BeginLayout();
    CLAY({ .id = CLAY_ID("OuterContainer"), .layout = { .sizing = {CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0)}, .padding = CLAY_PADDING_ALL(16), .childGap = 16 }, .backgroundColor = {250,250,255,255} }) {
        CLAY({
            .id = CLAY_ID("SideBarContainer"),
            .layout = { .sizing = { .width = CLAY_SIZING_FIXED(300), .height = CLAY_SIZING_GROW(0) }, .childGap = 8, .layoutDirection = CLAY_TOP_TO_BOTTOM },
        }) {
            CLAY({ .id = CLAY_ID("SideBarTitle"), .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(8), .childGap = 8, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } }, .backgroundColor = COLOR_TITLE }) {
                if (this->fileType == ROM) {
                    CLAY_TEXT(CLAY_STRING("Chip8 Roms"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                } else {
                    CLAY_TEXT(CLAY_STRING("Chip8 States"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                }
            }

            CLAY({
                .id = CLAY_ID("SideBar"),
                .layout = { .sizing = { .width = CLAY_SIZING_FIXED(300), .height = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(8), .childGap = 8, .layoutDirection = CLAY_TOP_TO_BOTTOM },
                .backgroundColor = COLOR_BOX,
                .clip = { .vertical = true, .childOffset = Clay_GetScrollOffset() }
            }) {
                std::unordered_map<std::string *, std::vector<std::string>>& files = (this->fileType == ROM) ? this->romFiles : this->stateFiles;
                if (this->romDirectories.empty() || files.empty()) {
                    CLAY_TEXT(CLAY_STRING("No files found"), CLAY_TEXT_CONFIG({ .textColor = {0, 0, 0, 255}, .fontSize = 24, .textAlignment = CLAY_TEXT_ALIGN_CENTER }));
                } else {
                    for (auto& dir : files) {
                        for (std::string& romPath : dir.second) {
                            if (&romPath == this->selectedRom) {
                                CLAY({.layout = {.sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(50) }, .padding = Clay_Padding({.left = 8, .top = 16, .bottom = 16}),.childGap = 16,.childAlignment = { .y = CLAY_ALIGN_Y_CENTER }},.backgroundColor = COLOR_HIGHLIGHT}) {
                                    CLAY_TEXT(toClayString(getFileFromStringPath(romPath)), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24, .wrapMode = CLAY_TEXT_WRAP_NONE }));
                                    Clay_OnHover(handleRomClick, reinterpret_cast<intptr_t>(&this->dataList.emplace_back(this, &romPath)));
                                }
                            } else {
                                CLAY({.layout = {.sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(50) }, .padding = Clay_Padding({.left = 8, .top = 16, .bottom = 16}),.childGap = 16,.childAlignment = { .y = CLAY_ALIGN_Y_CENTER }},.backgroundColor = COLOR_ROMS}) {
                                    CLAY_TEXT(toClayString(getFileFromStringPath(romPath)), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24, .wrapMode = CLAY_TEXT_WRAP_NONE }));
                                    Clay_OnHover(handleRomClick, reinterpret_cast<intptr_t>(&this->dataList.emplace_back(this, &romPath)));
                                }
                            }
                        }
                    }
                }
            }

            CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .childGap = 8} }) {
                CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(8), .childGap = 8, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } }, .backgroundColor = COLOR_BUTTON }) {
                    CLAY_TEXT(CLAY_STRING("Add New"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                    Clay_OnHover(handleAddNewRom, reinterpret_cast<intptr_t>(&this->dataList.emplace_back(this, nullptr)));
                }
                CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(8), .childGap = 8, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } }, .backgroundColor = COLOR_BUTTON }) {
                    CLAY_TEXT(CLAY_STRING("Refresh"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                    Clay_OnHover(handleRefresh, reinterpret_cast<intptr_t>(&this->dataList.emplace_back(this, nullptr)));
                }
            }
        }
        CLAY({ .id = CLAY_ID("MainContentHolder"),
            .layout = { .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) }, .childGap = 8, .layoutDirection = CLAY_TOP_TO_BOTTOM }}) {
            CLAY({ .id = CLAY_ID("MainContent"),
                .layout = { .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(8), .childGap = 8, .layoutDirection = CLAY_TOP_TO_BOTTOM },
                .backgroundColor = COLOR_BOX }) {
                if (this->selectedRom != nullptr) {
                    CLAY_TEXT(CLAY_STRING("Settings"), CLAY_TEXT_CONFIG({ .textColor = {0, 0, 0, 255}, .fontSize = 24 }));

                    CLAY_TEXT(CLAY_STRING("Quirks"), CLAY_TEXT_CONFIG({ .textColor = {0, 0, 0, 255}, .fontSize = 24 }));
                    CLAY({.layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(8) }, .backgroundColor = COLOR_BUTTON }) {
                        if (this->romSettings.shift) {
                            CLAY_TEXT(CLAY_STRING("Shift - Enabled"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                        } else {
                            CLAY_TEXT(CLAY_STRING("Shift - Disabled"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                        }
                        Clay_OnHover([](Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData) {
                            if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
                                const auto data = reinterpret_cast<HoverData*>(userData);
                                data->self->romSettings.shift = !data->self->romSettings.shift;
                            }
                        }, reinterpret_cast<intptr_t>(&this->dataList.emplace_back(this, nullptr)));
                    }
                    CLAY({.layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(8) }, .backgroundColor = COLOR_BUTTON }) {
                        if (this->romSettings.memoryIncrementByX) {
                            CLAY_TEXT(CLAY_STRING("memoryIncrementByX - Enabled"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                        } else {
                            CLAY_TEXT(CLAY_STRING("memoryIncrementByX - Disabled"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                        }
                        Clay_OnHover([](Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData) {
                            if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
                                const auto data = reinterpret_cast<HoverData*>(userData);
                                data->self->romSettings.memoryIncrementByX = !data->self->romSettings.memoryIncrementByX;
                            }
                        }, reinterpret_cast<intptr_t>(&this->dataList.emplace_back(this, nullptr)));
                    }
                    CLAY({.layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(8) }, .backgroundColor = COLOR_BUTTON }) {
                        if (this->romSettings.memoryLeaveIUnchanged) {
                            CLAY_TEXT(CLAY_STRING("memoryLeaveIUnchanged - Enabled"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                        } else {
                            CLAY_TEXT(CLAY_STRING("memoryLeaveIUnchanged - Disabled"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                        }
                        Clay_OnHover([](Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData) {
                            if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
                                const auto data = reinterpret_cast<HoverData*>(userData);
                                data->self->romSettings.memoryLeaveIUnchanged = !data->self->romSettings.memoryLeaveIUnchanged;
                            }
                        }, reinterpret_cast<intptr_t>(&this->dataList.emplace_back(this, nullptr)));
                    }
                    CLAY({.layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(8) }, .backgroundColor = COLOR_BUTTON }) {
                        if (this->romSettings.wrap) {
                            CLAY_TEXT(CLAY_STRING("wrap - Enabled"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                        } else {
                            CLAY_TEXT(CLAY_STRING("wrap - Disabled"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                        }
                        Clay_OnHover([](Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData) {
                            if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
                                const auto data = reinterpret_cast<HoverData*>(userData);
                                data->self->romSettings.wrap = !data->self->romSettings.wrap;
                            }
                        }, reinterpret_cast<intptr_t>(&this->dataList.emplace_back(this, nullptr)));
                    }
                    CLAY({.layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(8) }, .backgroundColor = COLOR_BUTTON }) {
                        if (this->romSettings.jump) {
                            CLAY_TEXT(CLAY_STRING("jump - Enabled"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                        } else {
                            CLAY_TEXT(CLAY_STRING("jump - Disabled"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                        }
                        Clay_OnHover([](Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData) {
                            if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
                                const auto data = reinterpret_cast<HoverData*>(userData);
                                data->self->romSettings.jump = !data->self->romSettings.jump;
                            }
                        }, reinterpret_cast<intptr_t>(&this->dataList.emplace_back(this, nullptr)));
                    }
                    CLAY({.layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(8) }, .backgroundColor = COLOR_BUTTON }) {
                        if (this->romSettings.vblank) {
                            CLAY_TEXT(CLAY_STRING("vblank - Enabled"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                        } else {
                            CLAY_TEXT(CLAY_STRING("vblank - Disabled"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                        }
                        Clay_OnHover([](Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData) {
                            if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
                                const auto data = reinterpret_cast<HoverData*>(userData);
                                data->self->romSettings.vblank = !data->self->romSettings.vblank;
                            }
                        }, reinterpret_cast<intptr_t>(&this->dataList.emplace_back(this, nullptr)));
                    }
                    CLAY({.layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(8) }, .backgroundColor = COLOR_BUTTON }) {
                        if (this->romSettings.logic) {
                            CLAY_TEXT(CLAY_STRING("logic - Enabled"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                        } else {
                            CLAY_TEXT(CLAY_STRING("logic - Disabled"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                        }
                        Clay_OnHover([](Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData) {
                            if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
                                const auto data = reinterpret_cast<HoverData*>(userData);
                                data->self->romSettings.logic = !data->self->romSettings.logic;
                            }
                        }, reinterpret_cast<intptr_t>(&this->dataList.emplace_back(this, nullptr)));
                    }

                    CLAY_TEXT(CLAY_STRING("Keybindings"), CLAY_TEXT_CONFIG({ .textColor = {0, 0, 0, 255}, .fontSize = 24 }));
                    CLAY({.layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(8) }, .backgroundColor = COLOR_BUTTON }) {
                        CLAY_TEXT(CLAY_STRING("Configure"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                        Clay_OnHover([](Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData) {
                            if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
                                const auto data = reinterpret_cast<HoverData*>(userData);
                                if (!data->self->isKeymapMenuOpen) {
                                    std::string selectedRomSha1 = sha1FromFile(*data->self->selectedRom);
                                    data->self->windows.emplace_back(std::make_unique<KeybindingsMenu>(data->self->fonts[0], selectedRomSha1, &data->self->isKeymapMenuOpen))->init();
                                }
                            }
                        }, reinterpret_cast<intptr_t>(&this->dataList.emplace_back(this, nullptr)));
                    }

                    CLAY_TEXT(CLAY_STRING("State"), CLAY_TEXT_CONFIG({ .textColor = {0, 0, 0, 255}, .fontSize = 24 }));
                    CLAY({.layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(8) }, .backgroundColor = COLOR_BUTTON }) {
                        if (this->selectedState == nullptr) {
                            CLAY_TEXT(CLAY_STRING("Choose a state"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                            Clay_OnHover([](Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData) {
                                if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
                                    SDL_DialogFileFilter stateFilter[] = { {"Chip8 State Files", "state"} };
                                    const auto data = reinterpret_cast<HoverData*>(userData);
                                    SDL_ShowOpenFileDialog(selectStateCallback, data->self, reinterpret_cast<MainMenu*>(userData)->window, stateFilter, 1, home, false);
                                }
                            }, reinterpret_cast<intptr_t>(&this->dataList.emplace_back(this, nullptr)));
                        } else {
                            std::string selectedStateName = getFileFromStringPath(*this->selectedState);
                            CLAY_TEXT(toClayString(selectedStateName), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                            Clay_OnHover([](Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData) {                                
                                if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
                                    const auto data = reinterpret_cast<HoverData*>(userData);
                                    data->self->selectedState = nullptr;
                                }
                            }, reinterpret_cast<intptr_t>(&this->dataList.emplace_back(this, nullptr)));
                        }
                    }
                }
            }
            CLAY({.layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(8) }, .backgroundColor = COLOR_BUTTON }) {
                if (this->selectedRom == nullptr) {
                    CLAY_TEXT(CLAY_STRING("Please select a ROM"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                } else {
                    CLAY_TEXT(CLAY_STRING("Play"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                    Clay_OnHover(handlePlay, reinterpret_cast<intptr_t>(&this->dataList.emplace_back(this, nullptr)));
                }
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

void MainMenu::resize(SDL_Event& event) {
    this->width = event.window.data1;
    this->height = event.window.data2;
}

void MainMenu::close() {
    for (const auto& window : this->windows) {
        if (window.get() == this) {
            continue;
        }
        window->close();
    }
    TTF_DestroyRendererTextEngine(this->textEngine);
    SDL_DestroyMutex(this->mutex);
    Window::close();
}

void MainMenu::handleRomClick(Clay_ElementId elementId, const Clay_PointerData pointerData, const intptr_t userData) {
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        const auto data = reinterpret_cast<HoverData*>(userData);
        data->self->selectedRom = data->data;
    }
}

void MainMenu::handleAddNewRom(Clay_ElementId elementId, const Clay_PointerData pointerData, const intptr_t userData) {
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        const auto data = reinterpret_cast<HoverData*>(userData);
        SDL_ShowOpenFolderDialog(callback, data->self, data->self->window, home, false);
    }
}

void MainMenu::handlePlay(Clay_ElementId elementId, const Clay_PointerData pointerData, const intptr_t userData) {
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        const auto data = reinterpret_cast<HoverData*>(userData);
        if (data->self->selectedRom == nullptr) {
            return;
        }
        std::unordered_map<uint8_t, unsigned char> romKeymap = data->self->getSelectedRomKeymap();
        if (data->self->selectedState != nullptr) {
            std::cout << *data->self->selectedState << std::endl;
            data->self->windows.emplace_back(std::make_unique<Emulator>(*data->self->selectedState, data->self->romSettings, romKeymap))->init();
        } else {
            data->self->windows.emplace_back(std::make_unique<Emulator>(*data->self->selectedRom, data->self->romSettings, romKeymap))->init();
        }
    }
}

std::unordered_map<uint8_t, unsigned char> MainMenu::getSelectedRomKeymap() {
    nlohmann::json json;
    if (std::ifstream file(configFilePath); file.good()) {
        json = nlohmann::json::parse(file);
        file.close();
    }

    if (this->selectedRom == nullptr) {
        return defaultKeymap;
    }

    std::string selectedRomSha1 = sha1FromFile(*this->selectedRom);
    if (json.contains("keymaps") && json["keymaps"].contains(selectedRomSha1)) {
        std::unordered_map<uint8_t, unsigned char> romKeymap = json["keymaps"].at(selectedRomSha1);
        return romKeymap;
    } else {
        return defaultKeymap;
    }
}

void MainMenu::handleRefresh(Clay_ElementId elementId, const Clay_PointerData pointerData, const intptr_t userData) {
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        const auto data = reinterpret_cast<HoverData*>(userData);
        data->self->romFiles.clear();
        data->self->stateFiles.clear();
        for (const auto& dir : data->self->romDirectories) {
            searchDirectory(dir, data->self->romFiles, data->self->stateFiles, data->self->romDirectories);
        }
    }
}

void MainMenu::callback(void* userdata, const char* const* directory, int filter) {
    if (!directory) {
        SDL_Log("An error occurred: %s", SDL_GetError());
        return;
    }
    if (!*directory) {
        SDL_Log("The user did not select any file. Most likely, the dialog was canceled.");
        return;
    }

    auto* instance = static_cast<MainMenu*>(userdata);

    std::string directoryString = *directory;
    std::cout << "Selected directory \"" << directoryString << "\"" << std::endl;

    if (directoryString.empty()) {
        SDL_Log("The user did not select any file. Most likely, the dialog was canceled.");
        return;
    }

    SDL_LockMutex(instance->mutex);

    nlohmann::json json;
    if (std::ifstream file(configFilePath); file.good()) {
        json = nlohmann::json::parse(file);
        file.close();
    }

    json["directories"].push_back(directoryString);

    std::ofstream fileWrite(configFilePath);
    fileWrite << json.dump(4);
    fileWrite.close();

    instance->romDirectories.emplace_back(directoryString);
    searchDirectory(directoryString, instance->romFiles, instance->stateFiles, instance->romDirectories);
    SDL_UnlockMutex(instance->mutex);
}

void MainMenu::selectStateCallback(void* userdata, const char* const* selectedFile, int filter) {
    if (!selectedFile) {
        SDL_Log("An error occurred: %s", SDL_GetError());
        return;
    }
    if (!*selectedFile) {
        SDL_Log("The user did not select any file. Most likely, the dialog was canceled.");
        return;
    }

    auto* instance = static_cast<MainMenu*>(userdata);

    std::string selectedFilePathString = *selectedFile;
    if (selectedFilePathString.empty()) {
        SDL_Log("The user did not select any file. Most likely, the dialog was canceled.");
        return;
    }

    std::ifstream fileReader;
    fileReader.open(selectedFilePathString, std::ios::binary);
    
    if (!fileReader.is_open()) {
        std::cerr << "Error opening file '" << selectedFilePathString << "'" << std::endl;
        return;
    }

    std::vector<uint8_t> buffer(sha1Dimension);

    fileReader.seekg(0, std::ios::end);
    size_t file_size = fileReader.tellg();
    fileReader.seekg(Cpu::serializationDimension - 1, std::ios::beg);
    
    fileReader.read(reinterpret_cast<char*>(&buffer[0]), sha1Dimension);
    fileReader.close();

    std::string stateSha1(buffer.begin(), buffer.end());

    if (stateSha1.compare(sha1FromFile(*instance->selectedRom)) != 0) {
        std::cerr << "The selected state does not belong to the selected ROM!" << std::endl;
        std::cerr << "State SHA1: " << stateSha1 << std::endl;
        std::cerr << "ROM SHA1: " << sha1FromFile(*instance->selectedRom) << std::endl;
        // TODO integrate with database to show the ROM name
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "The selected state does not belong to the selected ROM!", instance->window);
    } else {
        instance->selectedState = new std::string(selectedFilePathString);
    }
}
