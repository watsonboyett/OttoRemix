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
            buffer[i].freq_Hz = 0;
            buffer[i].duration_ms = 0;
        }
    }

    void AppendNote(MusicalNote note)
    {
        if (write_idx < NOTES_MAX)
        {
            buffer[write_idx].freq_Hz = note.freq_Hz;
            buffer[write_idx].duration_ms = note.duration_ms;
            write_idx++;
        }
    }

    bool GetNextNote(MusicalNote * note)
    {
        if (read_idx < write_idx)
        {
            note->freq_Hz = buffer[read_idx].freq_Hz;
            note->duration_ms = buffer[read_idx].duration_ms;
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
