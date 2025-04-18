#include "Window.h"

#include <iostream>

Window::Window() {
}

Window::~Window() {

}

void Window::init() {
    init(this->width, this->height);
}

void Window::init(const int width, const int height) {
    if (!SDL_CreateWindowAndRenderer("8ChocChip - CHIP-8 Emulator", width, height, SDL_WINDOW_RESIZABLE, &this->window, &this->renderer)) {
        std::cerr << "Couldn't create window and renderer. SDL_Error: " << SDL_GetError() << std::endl;
        return;
    }

    // Set window icon
    SDL_Surface* icon = SDL_LoadBMP("assets/icon.bmp");
    if (!icon) {
        std::cerr << "Icon could not be loaded! SDL_Error: " << SDL_GetError() << std::endl;
        return;
    }
    SDL_SetWindowIcon(this->window, icon);
    SDL_DestroySurface(icon);

    this->windowId = SDL_GetWindowID(this->window);
    this->shown = true;

    SDL_GetWindowSize(this->window, &this->originalSize.x, &this->originalSize.y);
}

bool Window::handleEvent(SDL_Event &event) {
    if (event.window.windowID == this->windowId) { // TODO: Maybe let it go one more frame?
        switch (event.type) {
            case SDL_EVENT_WINDOW_SHOWN:
                this->shown = true;
            break;
            case SDL_EVENT_WINDOW_HIDDEN:
                this->shown = false;
            break;
            case SDL_EVENT_WINDOW_RESIZED:
                resize(event);
            break;
            case SDL_EVENT_WINDOW_EXPOSED:
                SDL_RenderPresent(this->renderer);
            break;
            case SDL_EVENT_WINDOW_MOUSE_ENTER:
                this->mouseFocus = true;
            break;
            case SDL_EVENT_WINDOW_MOUSE_LEAVE:
                this->mouseFocus = false;
            break;
            case SDL_EVENT_WINDOW_FOCUS_GAINED:
                this->keyboardFocus = true;
            break;
            case SDL_EVENT_WINDOW_FOCUS_LOST:
                this->keyboardFocus = false;
            break;
            case SDL_EVENT_WINDOW_MINIMIZED:
                this->minimised = true;
            break;
            case SDL_EVENT_WINDOW_MAXIMIZED:
                this->minimised = false;
            break;
            case SDL_EVENT_WINDOW_RESTORED:
                this->minimised = false;
            break;
            case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                SDL_HideWindow(this->window);
                close();
            break;
        }

        return true;
    }

    return false;
}


void Window::update() {
}

void Window::resize(SDL_Event &event) {

}

void Window::close() {
    SDL_DestroyRenderer(this->renderer);
    SDL_DestroyWindow(this->window);
    this->destroyed = true;
}

bool Window::isShown() const {
    return true;
}

bool Window::isDestroyed() const {
    return this->destroyed;
}

