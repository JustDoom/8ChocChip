#ifndef TIMER_H
#define TIMER_H

#include <SDL3/SDL.h>

class Timer {
public:
    //Initializes variables
    Timer();

    //The various clock actions
    void start();
    void stop();
    void pause();
    void unpause();

    //Gets the timer's time
    Uint64 getTicks();

    //Checks the status of the timer
    bool isStarted();
    bool isPaused();

private:
    //The clock time when the timer started
    Uint64 mStartTicks;

    //The ticks stored when the timer was paused
    Uint64 mPausedTicks;

    //The timer status
    bool mPaused;
    bool mStarted;
};

#endif