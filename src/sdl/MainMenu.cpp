#include "MainMenu.h"

#include <filesystem>
#include <fstream>
#include <iostream>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

#include "../util/Constants.h"
#include "../util/MiscUtil.h"
#include "Emulator.h"

#define WIDTH (64 * 15)
#define HEIGHT (32 * 15)

constexpr auto COLOR_LIGHT = (Clay_Color) {224, 215, 210, 255};
constexpr auto COLOR_RED = (Clay_Color) {168, 66, 28, 255};
constexpr auto COLOR_ORANGE = (Clay_Color) {225, 138, 50, 255};
struct HoverData {
    MainMenu* self;
    SDL_Window* window;
    std::string* romPath;
};
bool clicked = false;
Clay_SDL3RendererData rendererData;
Clay_Vector2 size{WIDTH, HEIGHT};
Clay_Vector2 wheel{};
std::vector<std::unique_ptr<char[]>> clayStringBuffers;
void clearClayStringBuffers() {
    clayStringBuffers.clear();
}

Clay_String toClayString(const std::string& str) {
    const size_t requiredSize = str.size() + 1;
    auto buffer = std::make_unique<char[]>(requiredSize);
    std::strcpy(buffer.get(), str.c_str());

    const Clay_String clayStr = {
        .length = static_cast<int32_t>(str.size()),
        .chars = buffer.get()
    };

    clayStringBuffers.push_back(std::move(buffer));
    return clayStr;
}
static Clay_Dimensions SDL_MeasureText(Clay_StringSlice text, Clay_TextElementConfig *config, void *userData) {
    TTF_Font **fonts = (TTF_Font**) userData;
    TTF_Font *font = fonts[config->fontId];
    int width, height;

    if (!TTF_GetStringSize(font, text.chars, text.length, &width, &height)) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to measure text: %s", SDL_GetError());
    }

    return (Clay_Dimensions) { (float) width, (float) height };
}

MainMenu::MainMenu(TTF_Font* font, std::unordered_map<std::string *, std::vector<std::string>> &romFiles,
                   std::vector<std::string> &romDirectories, std::vector<std::unique_ptr<Window>> &windows) :
    romDirectories(romDirectories), romFiles(romFiles), windows(windows), font(font), mutex(SDL_CreateMutex()) {}

void HandleClayErrors(Clay_ErrorData errorData) {
    printf("%s", errorData.errorText.chars);
}

void MainMenu::init() {
    Window::init();

    this->textEngine = TTF_CreateRendererTextEngine(this->renderer);
    rendererData.renderer = this->renderer;
    rendererData.textEngine = this->textEngine;
    rendererData.fonts = (TTF_Font**) SDL_calloc(1, sizeof(TTF_Font *));
    rendererData.fonts[0] = font;
    uint64_t totalMemorySize = Clay_MinMemorySize();
    Clay_Arena clayMemory = (Clay_Arena) {
        .capacity = totalMemorySize,
        .memory = (char*) SDL_malloc(totalMemorySize)
    };

    int width, height;
    SDL_GetWindowSize(this->window, &width, &height);
    Clay_Initialize(clayMemory, (Clay_Dimensions) { (float) width, (float) height }, (Clay_ErrorHandler) { HandleClayErrors });
    Clay_SetMeasureTextFunction(SDL_MeasureText, rendererData.fonts);
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
    Clay_SetLayoutDimensions((Clay_Dimensions) { size.x, size.y });
    Clay_SetPointerState((Clay_Vector2) { x, y }, clicked);
    Clay_UpdateScrollContainers(true, (Clay_Vector2) { wheel.x, wheel.y }, 0.0166f);
    wheel.x = 0;
    wheel.y = 0;

    Clay_BeginLayout();
    CLAY({ .id = CLAY_ID("OuterContainer"), .layout = { .sizing = {CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0)}, .padding = CLAY_PADDING_ALL(16), .childGap = 16 }, .backgroundColor = {250,250,255,255} }) {
        CLAY({
            .id = CLAY_ID("SideBar"),
            .layout = { .sizing = { .width = CLAY_SIZING_FIXED(300), .height = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(8), .childGap = 8, .layoutDirection = CLAY_TOP_TO_BOTTOM },
            .backgroundColor = COLOR_LIGHT,
            .scroll = { .vertical = true }
        }) {
            CLAY({ .id = CLAY_ID("ProfilePictureOuter"), .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(8), .childGap = 8, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } }, .backgroundColor = COLOR_RED }) {
                CLAY_TEXT(CLAY_STRING("Game Roms"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
            }

            for (auto& dir : this->romFiles) {
                for (std::string& romPath : dir.second) {
                    CLAY({.layout = {.sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(50) },.padding = Clay_Padding({.left = 8, .top = 16, .bottom = 16}),.childGap = 16,.childAlignment = { .y = CLAY_ALIGN_Y_CENTER }},.backgroundColor = COLOR_ORANGE}) {
                        CLAY_TEXT(toClayString(getFileFromPath(romPath)), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24, .wrapMode = CLAY_TEXT_WRAP_NONE }));
                        HoverData* data = new HoverData{this, this->window, &romPath};
                        Clay_OnHover([](Clay_ElementId elementId, const Clay_PointerData pointerData, const intptr_t userData) {
                            if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
                                const auto data = reinterpret_cast<HoverData*>(userData);
                                data->self->windows.emplace_back(std::make_unique<Emulator>(*data->romPath))->init();
                            }
                        }, reinterpret_cast<intptr_t>(data));
                    }
                }
            }

            CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(8), .childGap = 8, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } }, .backgroundColor = (Clay_Color) {224, 25, 210, 255} }) {
                CLAY_TEXT(CLAY_STRING("Add New"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontSize = 24 }));
                HoverData* data = new HoverData{this, this->window, nullptr};
                Clay_OnHover([](Clay_ElementId elementId, const Clay_PointerData pointerData, const intptr_t userData) {
                    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
                        const auto data = reinterpret_cast<HoverData*>(userData);
                        SDL_ShowOpenFolderDialog(data->self->callback, data->self, data->window, home, false);
                    }
                }, reinterpret_cast<intptr_t>(data));
            }

            CLAY({ .id = CLAY_ID("MainContent"), .layout = { .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) } }, .backgroundColor = COLOR_LIGHT }) {}
        }
    }
    Clay_RenderCommandArray renderCommands = Clay_EndLayout();

    SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 255);
    SDL_RenderClear(this->renderer);

    SDL_Clay_RenderClayCommands(&rendererData, &renderCommands);
    clearClayStringBuffers();

    SDL_RenderPresent(this->renderer);
}

void MainMenu::resize(SDL_Event& event) {
    size.x = event.window.data1;
    size.y = event.window.data2;
}

void MainMenu::close() {
    for (const auto& window : this->windows) {
        window->close();
    }
    TTF_DestroyRendererTextEngine(this->textEngine);
    SDL_DestroyMutex(this->mutex);
    Window::close();
}

void MainMenu::callback(void* userdata, const char* const* directory, int filter) {
    if (!directory) {
        SDL_Log("An error occured: %s", SDL_GetError());
        return;
    }
    if (!*directory) {
        SDL_Log("The user did not select any file. Most likely, the dialog was canceled.");
        return;
    }

    auto* instance = static_cast<MainMenu*>(userdata);

    std::cout << *directory << std::endl;
    std::string directoryString = *directory;

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

    for (const auto &file: std::filesystem::directory_iterator(directoryString)) {
        if (file.is_directory() || file.file_size() > 3584) {
            continue; // TODO: Make sure its a file that can be emulated, at least basic checks so it isn't like a word doc
        }

        std::cout << "Processing file - : " << file.path().c_str() << std::endl;

        // Check if the rom directory doesn't exist in romFiles, then add it
        if (instance->romFiles.find(&instance->romDirectories.back()) == instance->romFiles.end()) {
            instance->romFiles.emplace(&instance->romDirectories.back(), std::vector<std::string>());
        }

        // Add the file path to the romFiles entry
        instance->romFiles.find(&instance->romDirectories.back())->second.emplace_back(file.path().string());
    }

    SDL_UnlockMutex(instance->mutex);
}
