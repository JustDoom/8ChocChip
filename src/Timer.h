#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

class Timer {
public:
    Timer();

    void start();
    void stop();
    void pause();
    void unpause();

    uint64_t getTicks() const;

    bool isStarted() const;
    bool isPaused() const;

private:
    uint64_t startTicks;
    uint64_t pausedTicks;

    bool paused;
    bool started;
};

#endif