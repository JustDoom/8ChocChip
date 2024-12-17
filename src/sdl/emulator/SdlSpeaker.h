#ifndef INC_8CHOCCHIP_SDLSPEAKER_H
#define INC_8CHOCCHIP_SDLSPEAKER_H

#include "../../emulator/Speaker.h"

class SdlSpeaker : public Speaker {
public:
    SdlSpeaker();

    void play() override;
    void stop() override;
};

#endif