#ifndef INC_8CHOCCHIP_SDLSPEAKER_H
#define INC_8CHOCCHIP_SDLSPEAKER_H

#include <vector>

#include <SDL3/SDL_audio.h>

#include "../../emulator/Speaker.h"

class SdlSpeaker : public Speaker {
private:
    int amplitude;
    int sampleRate;
    int frequency;
    int duration;
    int sampleCount;

    SDL_AudioSpec audioSpec;
    SDL_AudioStream* audioStream;
    std::vector<int16_t> beepData;
public:
    SdlSpeaker();
    ~SdlSpeaker();

    void play() override;
    void stop() override;
    void generateSample();
};

#endif