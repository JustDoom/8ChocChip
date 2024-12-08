#include "SfmlSpeaker.h"

#include <cmath>

SfmlSpeaker::SfmlSpeaker() {

    this->sound.setVolume(100.f);
    this->sound.setLoop(true);

    constexpr unsigned int sampleRate = 44100;
    sf::Int16 samples[sampleRate];
    for (unsigned int i = 0; i < sampleRate; ++i) {
        constexpr unsigned int frequency = 440;
        samples[i] = static_cast<sf::Int16>(32767 * std::sin(2 * 3.14159265 * frequency * i / sampleRate)); // Mmmm yum pi
    }
    this->soundBuffer.loadFromSamples(samples, sampleRate, 1, sampleRate);

    this->sound.setBuffer(this->soundBuffer);
}

void SfmlSpeaker::play() {
    this->sound.play();
}

void SfmlSpeaker::stop() {
    this->sound.stop();
}

