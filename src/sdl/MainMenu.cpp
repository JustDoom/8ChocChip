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
#include "ui/ScrollBox.h"

#define WIDTH (64 * 15)
#define HEIGHT (32 * 15)

MainMenu::MainMenu(TTF_Font* font, std::string configFilePath, std::unordered_map<std::string *, std::vector<std::string>> &romFiles,
                   std::vector<std::string> &romDirectories, std::vector<std::unique_ptr<Window>> &windows) :
    configFilePath(configFilePath), romDirectories(romDirectories), romFiles(romFiles), windows(windows), font(font),
    mutex(SDL_CreateMutex()) {}

void MainMenu::init() {
    Window::init();

    this->textEngine = TTF_CreateRendererTextEngine(this->renderer);

    this->mainBox = std::make_shared<MainBox>(WIDTH, HEIGHT);

    this->scrollRoms = std::make_shared<ScrollBox>(0, 0, WIDTH, 400, this->mainBox);
    this->scrollRoms->setElements(refreshRoms());
    this->mainBox->getElements().emplace_back(this->scrollRoms);

    TTF_Text* text = TTF_CreateText(this->textEngine, this->font, "Select ROM", 0);
    if (!text) {
        SDL_Log("Failed to create text: %s\n", SDL_GetError());
        return;
    }

    this->chooseFolder = std::make_shared<TextButton>(0.0f, 400.0f, WIDTH, 80, text, this->mainBox, SDL_Color{150, 150, 150, 255});
    this->chooseFolder->setOnClick([this]() {
        SDL_ShowOpenFolderDialog(callback, this, this->window, home, false);
    });
    this->mainBox->getElements().emplace_back(this->chooseFolder);
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

    for (const std::shared_ptr<Element>& element : this->mainBox->getElements()) {
        element->handleEvent(event);
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

    this->mainBox->update(this->inputHandler);

    this->inputHandler.updateLastKeys();
    this->inputHandler.updateLastMouse();
}

void MainMenu::render() {
    SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 255);
    SDL_RenderClear(this->renderer);

    this->mainBox->draw(this->renderer);

    SDL_RenderPresent(this->renderer);
}

void MainMenu::resize(SDL_Event& event) {
    SDL_Point size;
    SDL_GetWindowSize(this->window, &size.x, &size.y);

    SDL_FPoint point{};
    SDL_GetMouseState(&point.x, &point.y);

    // for (auto& romButton : this->roms) {
    //     romButton.second.updateSize(this->originalSize, size);
    //     romButton.second.update(this->inputHandler, point);
    // }
    //
    // this->chooseFolder->updateSize(this->originalSize,size);
    // this->chooseFolder->update(this->inputHandler, point);
}

void MainMenu::close() {
    for (const auto& window : this->windows) {
        window->close();
    }
    TTF_DestroyRendererTextEngine(this->textEngine);
    SDL_DestroyMutex(this->mutex);
    Window::close();
}

void MainMenu::launchRom(const std::string &file) const {
    this->windows.emplace_back(std::make_unique<Emulator>(file))->init();
}

std::vector<std::shared_ptr<Element>> MainMenu::refreshRoms() {
    std::vector<std::shared_ptr<Element>> scrollBoxRoms;
    int count = 0;
    for (auto& thing: this->romFiles) {
        for (std::string& file: thing.second) {
            TTF_Text* text = TTF_CreateText(this->textEngine, this->font, getFileFromPath(file).c_str(), 0);
            if (!text) {
                SDL_Log("Failed to create text: %s\n", SDL_GetError());
                continue;
            }

            auto button = std::shared_ptr<TextButton>();
            if (count++ & 1) {
                button = std::make_shared<TextButton>(0, 25 * scrollBoxRoms.size(), WIDTH, 25, text, this->scrollRoms, SDL_Color{175, 175, 175, 255});
            } else {
                button = std::make_shared<TextButton>(0, 25 * scrollBoxRoms.size(), WIDTH, 25, text, this->scrollRoms);
            }
        //     button->setOnClick([this, button, &file]() {
        //         launchRom(file);
        //         if (button->isHovered()) {
        //             if (inputHandler.isPressed(SDL_SCANCODE_LSHIFT)) {
        //                 updateColour(this->deleteColor);
        //                 if (inputHandler.isJustClicked(SDL_BUTTON_LEFT)) {
        //                     button.destroy();
        //                 }
        //             } else if (inputHandler.isJustClicked(SDL_BUTTON_LEFT)) {
        //                 this->isPressed = true;
        //                 updateColour(this->activeColor);
        //                 if (this->onClick) {
        //                     this->onClick();
        //                 }
        //             } else {
        //                 updateColour(this->hoverColor);
        //             }
        //         } else {
        //             this->isPressed = false;
        //             updateColour(this->idleColor);
        //         }
        //     });
        scrollBoxRoms.emplace_back(button);
        }
    }
    return scrollBoxRoms;
}

void MainMenu::callback(void* userdata, const char* const* directory, int filter) {
    if (!directory) {
        SDL_Log("An error occured: %s", SDL_GetError());
        return;
    }
    if (!*directory) {
        SDL_Log("The user did not select any file.");
        SDL_Log("Most likely, the dialog was canceled.");
        return;
    }

    auto* instance = static_cast<MainMenu*>(userdata);

    std::cout << *directory << std::endl;
    std::string directoryString = *directory;

    SDL_LockMutex(instance->mutex);

    nlohmann::json json;
    if (std::ifstream file(instance->configFilePath); file.good()) {
        json = nlohmann::json::parse(file);
        file.close();
    }

    json["directories"].push_back(directoryString);

    std::ofstream fileWrite(instance->configFilePath);
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

        TTF_Text *text = TTF_CreateText(instance->textEngine, instance->font, file.path().filename().string().c_str(), 0);
        if (!text) {
            SDL_Log("Failed to create text: %s\n", SDL_GetError());
            return;
        }

        instance->roms.emplace(file.path().string(), TextButton(0, 25.0f * instance->roms.size(), WIDTH, 25, text));
    }
    instance->scrollRoms->setElements(instance->refreshRoms());

    SDL_UnlockMutex(instance->mutex);
}
