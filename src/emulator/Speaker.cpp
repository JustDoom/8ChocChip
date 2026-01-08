#include "Speaker.h"

#include <cmath>
#include <iostream>

Speaker::Speaker() : amplitude(28000), sampleRate(44100), frequency(220), duration(500), sampleCount((sampleRate * duration) / 1000), beepData(sampleCount) {
    SDL_AudioSpec desiredSpec;
    SDL_zero(desiredSpec);
    desiredSpec.freq = this->sampleRate;
    desiredSpec.format = SDL_AUDIO_S16;
    desiredSpec.channels = 1;

    this->audioStream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &desiredSpec, nullptr, nullptr);
    if (this->audioStream == nullptr) {
        std::cerr << "Failed to open audio device stream: " << SDL_GetError() << std::endl;
        return;
    }

    generateSample();
}

Speaker::~Speaker() {
    if (this->audioStream != nullptr) {
        SDL_DestroyAudioStream(this->audioStream);
    }
}

void Speaker::play() {
    if (this->audioStream != nullptr) {
        if (SDL_GetAudioStreamAvailable(this->audioStream) == 0) {
            generateSample();
        }
        SDL_ResumeAudioStreamDevice(this->audioStream);
    }
}

void Speaker::stop() const {
    if (this->audioStream != nullptr) {
        SDL_PauseAudioStreamDevice(this->audioStream);
    }
}

void Speaker::generateSample() {
    for (int i = 0; i < this->sampleCount; ++i) {
        this->beepData[i] = static_cast<int16_t>(this->amplitude * std::sin(2.0 * std::acos(-1) * this->frequency * i / this->sampleRate));
    }
    if (SDL_PutAudioStreamData(this->audioStream, this->beepData.data(), this->beepData.size() * sizeof(int16_t)) < 0) {
        std::cerr << "Failed to put data into audio stream: " << SDL_GetError() << std::endl;
    }
}