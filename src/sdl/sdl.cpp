#include "sdl.h"

#include <fstream>
#include <iostream>
#include <thread>

#include "../emulator/Cpu.h"
#include "emulator/SdlKeyboard.h"
#include "emulator/SdlRenderer.h"
#include "emulator/SdlSpeaker.h"

#include "SDL2/SDL.h"

int sdl::launch(const std::string &rom) {
    if (rom.empty()) {
        std::cerr << "No ROM file has been specified :(" << std::endl;
        return 1;
    }

    std::ifstream file(rom, std::ios::binary | std::ios::ate);
    if (!file.good()) {
        std::cerr << "Can not find file " << rom << std::endl;
        return 1;
    }

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Window *window = SDL_CreateWindow("8ChocChip - CHIP-8 Emulator", SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED, 64 * 15, 32 * 15, SDL_WINDOW_SHOWN);

    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Set window icon
    SDL_Surface *icon = SDL_LoadBMP("../../assets/icon.bmp");
    if (icon == nullptr) {
        std::cerr << "Icon could not be loaded! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }
    SDL_SetWindowIcon(window, icon);
    SDL_FreeSurface(icon);

    // Create renderer
    SDL_Renderer *windowRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (windowRenderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    SdlRenderer renderer(windowRenderer);
    SdlSpeaker speaker;
    SdlKeyboard keyboard;
    Cpu cpu(&renderer, &keyboard, &speaker);

    cpu.loadSpritesIntoMemory();

    cpu.loadProgramIntoMemory(&file);

    bool focus;

    auto start = std::chrono::high_resolution_clock::now();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_seconds;

    // Variables to count frames
    int frames = 0;
    double fps;

    const int SCREEN_TICKS_PER_FRAME = 1000 / 60; // Time per frame in milliseconds

    Uint32 startTicks;

    while (true) {
        startTicks = SDL_GetTicks();

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                SDL_DestroyRenderer(windowRenderer);
                SDL_DestroyWindow(window);
                SDL_Quit();
                return 0;
            } else if (event.type == SDL_WINDOWEVENT) {
                if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
                    focus = true;
                } else if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
                    focus = false;
                }
            }

            if (!focus) continue;

            // Handle keyboard inputs
            if (event.type == SDL_KEYDOWN) {
                keyboard.handleKeyDown(event.key.keysym.sym);
            } else if (event.type == SDL_KEYUP) {
                keyboard.handleKeyUp(event.key.keysym.sym);
            }
        }

        // Run a cycle of the emulator
        cpu.cycle();

        // Set renderer draw color
        SDL_SetRenderDrawColor(windowRenderer, 0, 0, 0, 255);
        // Clear the window
        SDL_RenderClear(windowRenderer);

        // Render the window
        renderer.render();
        SDL_RenderPresent(windowRenderer);

        // Calculate time spent in frame
        int frameTicks = SDL_GetTicks() - startTicks;

        // If frame finished early
        if (frameTicks < SCREEN_TICKS_PER_FRAME) {
            // Wait remaining time
            SDL_Delay(SCREEN_TICKS_PER_FRAME - frameTicks);
        }

        frames++;

        end = std::chrono::high_resolution_clock::now();
        elapsed_seconds = end - start;

        // If elapsed time is greater than or equal to 1 second
        if (elapsed_seconds.count() >= 1.0) {
            // Calculate FPS
            fps = frames / elapsed_seconds.count();

            // Output FPS
            std::cout << "FPS: " << fps << std::endl;

            // Reset frame count and start time
            frames = 0;
            start = std::chrono::high_resolution_clock::now();
        }
    }
}