#define SDL_MAIN_USE_CALLBACKS  // This is necessary for the new callbacks API. To use the legacy API, don't define this.
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_main.h>

#include "EmulatorMain.h"

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
    auto* app = new EmulatorMain();
    if (!app->initialise(argc, argv)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to initialise program :(");
        return SDL_APP_FAILURE;
    }

    *appstate = app;

    SDL_Log("Application started successfully!");

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
    const auto app = static_cast<EmulatorMain*>(appstate);

    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }

    app->event(event);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    auto* app = static_cast<EmulatorMain*>(appstate);

    if (!app->isRunning()) {
        return SDL_APP_SUCCESS;
    }

    app->run();

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    if (const auto app = static_cast<EmulatorMain*>(appstate)) {
        app->quit();

        delete app;
    }
}