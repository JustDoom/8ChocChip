#include "MainMenu.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <thread>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <libconfig.h++>

#include "../util/MiscUtil.h"
#include "Emulator.h"

#define WIDTH (64 * 15)
#define HEIGHT (32 * 15)

MainMenu::MainMenu(TTF_Font* font, std::string configFilePath, std::unordered_map<std::string *, std::vector<std::string>> &romFiles,
                   std::vector<std::string> &romDirectories, std::vector<std::unique_ptr<Window>> &windows) :
    configFilePath(configFilePath), romDirectories(romDirectories), romFiles(romFiles), windows(windows), font(font) {}

void MainMenu::init() {
    Window::init();

    this->textEngine = TTF_CreateRendererTextEngine(this->renderer);

    for (auto& thing: this->romFiles) {
        for (std::string& file: thing.second) {

            TTF_Text* text = TTF_CreateText(textEngine, font, MiscUtil::getFileFromPath(file).c_str(), 0);
            if (!text) {
                SDL_Log("Failed to create text: %s\n", SDL_GetError());
                return;
            }

            TextButton romButton(0, 25.0f * this->roms.size(), WIDTH, 25, text);

            this->roms.emplace(file, romButton);
        }
    }

    TTF_Text* text = TTF_CreateText(textEngine, font, "Select ROM", 0);
    if (!text) {
        SDL_Log("Failed to create text: %s\n", SDL_GetError());
        return;
    }
    this->chooseFolder = std::make_unique<TextButton>(0, 400, WIDTH, 80, text);

    // font = TTF_OpenFont("font.ttf", 32); // Ensure the path is correct
    // if (!font) {
    //     // printf("TTF_OpenFont Error: %s\n", SDL_GetError());
    //     return;
    // }
    //
    // // Create the text surface
    // SDL_Color color = {0, 255, 0, 255}; // Green color
    // text = TTF_RenderText_Solid(font, "Some test", 9, color);
    // if (!text) {
    //     // printf("TTF_RenderText_Solid Error: %s\n", TTF_GetError());
    //     return;
    // }
    //
    // // Create the text texture
    // textTexture = SDL_CreateTextureFromSurface(renderer, text);
    // if (!textTexture) {
    //     printf("SDL_CreateTextureFromSurface Error: %s\n", SDL_GetError());
    //     SDL_DestroySurface(text); // Free the surface if texture creation fails
    //     return;
    // }
    //
    // SDL_DestroySurface(text);
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
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            this->inputHandler.addButton(event.button.button);
        break;
        case SDL_EVENT_MOUSE_BUTTON_UP:
            this->inputHandler.removeButton(event.button.button);
        break;
    }

    return true;
}

void MainMenu::update() {
    if (!this->mouseFocus) return;

    SDL_FPoint point{};
    SDL_GetMouseState(&point.x, &point.y);

    if (inputHandler.isJustPressed(SDL_SCANCODE_F3)) {
        this->debug = !this->debug;
    }

    for (auto &romButton: roms) {
        romButton.second.update(&this->inputHandler, point);

        if (!romButton.second.isJustClicked()) {
            continue;
        }

        this->windows.emplace_back(std::make_unique<Emulator>(romButton.first))->init();
    }
    this->chooseFolder->update(&this->inputHandler, point);

    if (this->chooseFolder->isJustClicked()) {

        if (nfdresult_t result = PickFolder(this->outPath); result == NFD_OKAY) {
            libconfig::Config config;
            config.readFile(this->configFilePath);

            libconfig::Setting &settings = config.getRoot();

            if (!settings.exists("directories")) {
                settings.add("directories", libconfig::Setting::TypeArray);
            }

            libconfig::Setting &directories = settings["directories"];
            directories.add(libconfig::Setting::TypeString) = outPath.get();

            romDirectories.emplace_back(outPath.get());

            for (const auto &file: std::filesystem::directory_iterator(outPath.get())) {
                if (file.is_directory()) {
                    continue; // TODO: Make sure its a file that can be emulated, at least basic checks so it isn't like
                    // a word doc
                }

                printf("Processing file - : %s\n", file.path().c_str());

                // Check if the rom directory doesn't exist in romFiles, then add it
                if (romFiles.find(&romDirectories.back()) == romFiles.end()) {
                    romFiles.emplace(&romDirectories.back(), std::vector<std::string>());
                }

                // Add the file path to the romFiles entry
                romFiles.find(&romDirectories.back())->second.emplace_back(file.path().string());

                TTF_Text* text = TTF_CreateText(textEngine, font, file.path().filename().string().c_str(), 0);
                if (!text) {
                    SDL_Log("Failed to create text: %s\n", SDL_GetError());
                    return;
                }

                TextButton romButton(0, 25.0f * roms.size(), WIDTH, 25, text);
                roms.emplace(file.path().string(), romButton);
            }
            config.writeFile(configFilePath);
        }
    }

    this->inputHandler.updateLastKeys();
    this->inputHandler.updateLastMouse();
}

void MainMenu::render() {
    SDL_SetRenderDrawColor(this->renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(renderer);
    for (auto &romButton: roms) {
        romButton.second.draw(renderer);
    }
    this->chooseFolder->draw(renderer);

    SDL_RenderPresent(renderer);
}

void MainMenu::resize(SDL_Event& event) {
    SDL_Point size;
    SDL_GetWindowSize(this->window, &size.x, &size.y);

    SDL_FPoint point{};
    SDL_GetMouseState(&point.x, &point.y);

    for (auto& romButton : roms) {
        romButton.second.updateSize(this->originalSize, size);
        romButton.second.update(&this->inputHandler, point);
    }

    chooseFolder->updateSize(this->originalSize,size);
    chooseFolder->update(&this->inputHandler, point);
}

void MainMenu::close() {
    for (const auto& window : this->windows) {
        window->close();
    }
    TTF_DestroyRendererTextEngine(this->textEngine);
    Window::close();
}

void MainMenu::refreshRoms() {

}
