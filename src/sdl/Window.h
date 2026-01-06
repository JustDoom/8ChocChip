#ifndef WINDOW_H
#define WINDOW_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

class Window {
protected:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer;
    TTF_TextEngine* textEngine;
    TTF_Font** fonts;
    unsigned int windowId = -1;
    SDL_Point originalSize;

    int width = 1280;
    int height = 720;

    bool mouseFocus = false;
    bool keyboardFocus = false;
    bool fullscreen = false;
    bool minimised = false;
    bool shown = false;
    bool destroyed = false;

    bool debug = false;
public:
    Window();
    virtual ~Window();

    virtual void init();
    virtual void init(int width, int height);
    virtual void init(SDL_Window* window, SDL_Renderer* renderer);
    virtual bool handleEvent(SDL_Event* event);
    virtual void update();
    virtual void render() = 0;
    virtual void resize(SDL_Event* event);
    virtual void close();

    bool isShown() const;
    bool isDestroyed() const;
};

#endif