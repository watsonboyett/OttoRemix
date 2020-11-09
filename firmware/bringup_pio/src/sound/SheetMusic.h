#pragma once

#include "MusicalNote.h"
#include <Arduino.h>

#define NOTES_MAX 120

class SheetMusic
{
public:
    SheetMusic() {}

    void Clear() 
    {
        write_idx = 0;
        read_idx = 0;

        for (int i = 0; i < NOTES_MAX; i++)
        {
            buffer[i].freq = 0;
            buffer[i].duration = 0;
        }
    }

    void AppendNote(MusicalNote note)
    {
        if (write_idx < NOTES_MAX)
        {
            buffer[write_idx].freq = note.freq;
            buffer[write_idx].duration = note.duration;
            write_idx++;
        }
    }

    bool GetNextNote(MusicalNote * note)
    {
        if (read_idx < write_idx)
        {
            note->freq = buffer[read_idx].freq;
            note->duration = buffer[read_idx].duration;
            read_idx++;

            return true;
        }
        
        return false;
    }

    void Restart()
    {
        read_idx = 0;
    }

private:
    MusicalNote buffer[NOTES_MAX];
    int write_idx = 0;
    int read_idx = 0;
};

class SheetMusicCatalog
{
public:
    static void LoadMarioTheme(SheetMusic * sheet, int octave, int bpm);
    static void LoadImperialMarch(SheetMusic * sheet, int octave, int bpm);
};
