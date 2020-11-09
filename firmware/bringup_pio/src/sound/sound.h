#pragma once

#include "SheetMusic.h"
#include <Arduino.h>

class Sound
{
public:
    Sound(int output_pin) : output_pin(output_pin) {}
    void play(float frequency, long noteDuration);
    void bend(float initFrequency, float finalFrequency, float prop, long noteDuration);

private:
    int output_pin;
};
