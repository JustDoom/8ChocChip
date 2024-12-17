#include "Timer.h"

#include <SDL3/SDL.h>

Timer::Timer() {
    this->startTicks = 0;
    this->pausedTicks = 0;

    this->paused = false;
    this->started = false;
}

void Timer::start() {
    this->started = true;
    this->paused = false;

    this->startTicks = SDL_GetTicks();
    this->pausedTicks = 0;
}

void Timer::stop() {
    this->started = false;
    this->paused = false;
    this->startTicks = 0;
    this->pausedTicks = 0;
}

void Timer::pause() {
    if(this->started && !this->paused) {
        this->paused = true;

        this->pausedTicks = SDL_GetTicks() - this->startTicks;
        this->startTicks = 0;
    }
}

void Timer::unpause() {
    if(this->started && this->paused) {
        this->paused = false;

        this->startTicks = SDL_GetTicks() - this->pausedTicks;

        this->pausedTicks = 0;
    }
}

uint64_t Timer::getTicks() const {
    uint64_t time = 0;

    if(this->started) {
        if(this->paused) {
            time = this->pausedTicks;
        } else {
            time = SDL_GetTicks() - this->startTicks;
        }
    }

    return time;
}

bool Timer::isStarted() const {
    return this->started;
}

bool Timer::isPaused() const {
    return this->paused && this->started;
}