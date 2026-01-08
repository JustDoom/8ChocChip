#ifndef SPEAKER_H
#define SPEAKER_H

#include <vector>

#include <SDL3/SDL.h>

class Speaker {
private:
    const int amplitude;
    const int sampleRate;
    const int frequency;
    const int duration;
    const int sampleCount;
    std::vector<int16_t> beepData;

    SDL_AudioSpec audioSpec;
    SDL_AudioStream* audioStream;
public:
    Speaker();
    ~Speaker();

    void play();
    void stop() const;
    void generateSample();
};

#endif