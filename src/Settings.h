#ifndef SETTINGS_H
#define SETTINGS_H

struct RomSettings {
    int tickSpeed;

    // Quirks
    bool shift = false;
    bool memoryIncrementByX = false;
    bool memoryLeaveIUnchanged = false;
    bool wrap = false;
    bool jump = false;
    bool vblank = true;
    bool logic = true;
};

#endif