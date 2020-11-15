#include "sound.h"

void Sound::play(float frequency, long noteDuration)
{
    if (frequency > 20)
    {
        tone(output_pin, frequency, noteDuration);
    }
    else
    {
        noTone(output_pin);
    }
    
}

void Sound::playWithDelay(float frequency, long noteDuration)
{
    if (frequency > 20)
    {
        tone(output_pin, frequency, noteDuration);
    }

    delay(noteDuration);
    noTone(output_pin);
}

void Sound::bend(float initFrequency, float finalFrequency, float prop, long noteDuration)
{
    if (initFrequency < finalFrequency)
    {
        for (int i = initFrequency; i < finalFrequency; i = i * prop)
        {
            tone(i, noteDuration);
        }
    }
    else
    {

        for (int i = initFrequency; i > finalFrequency; i = i / prop)
        {
            tone(i, noteDuration);
        }
    }
}
