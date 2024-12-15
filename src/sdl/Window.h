#ifndef WINDOW_H
#define WINDOW_H

#include <SDL3/SDL.h>

class Window {
protected:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    unsigned int windowId = -1;
    SDL_Point originalSize;

    int height = 720;
    int width = 480;

    bool mouseFocus = false;
    bool keyboardFocus = false;
    bool fullscreen = false;
    bool minimised = false;
    bool shown = false;

    bool debug = false;
public:
    Window();
    virtual ~Window();

    virtual void init();
    virtual bool handleEvent(SDL_Event& event);
    virtual void update();
    virtual void render() = 0;
    virtual void resize(SDL_Event& event);
    void close();

    bool isShown();
};

#endif