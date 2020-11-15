#include "SheetMusic.h"
#include "MusicalNote.h"

void SheetMusicCatalog::LoadImperialMarch(SheetMusic *sheet, int octave, int bpm)
{
    sheet->Clear();

    sheet->AppendNote(MusicalNote("G", octave, MusicalNote::QUARTER, bpm));
    sheet->AppendNote(MusicalNote("G", octave, MusicalNote::QUARTER, bpm));
    sheet->AppendNote(MusicalNote("G", octave, MusicalNote::QUARTER, bpm));
    sheet->AppendNote(MusicalNote("Eb", octave, 3 * MusicalNote::SIXTEENTH, bpm));
    sheet->AppendNote(MusicalNote("Bb", octave + 1, MusicalNote::SIXTEENTH, bpm));
    // --
    sheet->AppendNote(MusicalNote("G", octave, MusicalNote::QUARTER, bpm));
    sheet->AppendNote(MusicalNote("Eb", octave, 3 * MusicalNote::SIXTEENTH, bpm));
    sheet->AppendNote(MusicalNote("Bb", octave + 1, MusicalNote::SIXTEENTH, bpm));
    sheet->AppendNote(MusicalNote("G", octave, MusicalNote::HALF, bpm));
    // --
    sheet->AppendNote(MusicalNote("D", octave + 1, MusicalNote::QUARTER, bpm));
    sheet->AppendNote(MusicalNote("D", octave + 1, MusicalNote::QUARTER, bpm));
    sheet->AppendNote(MusicalNote("D", octave + 1, MusicalNote::QUARTER, bpm));
    sheet->AppendNote(MusicalNote("Eb", octave + 1, 3 * MusicalNote::SIXTEENTH, bpm));
    sheet->AppendNote(MusicalNote("Bb", octave + 1, MusicalNote::SIXTEENTH, bpm));
    // --
    sheet->AppendNote(MusicalNote("Gb", octave, MusicalNote::QUARTER, bpm));
    sheet->AppendNote(MusicalNote("Eb", octave, 3 * MusicalNote::SIXTEENTH, bpm));
    sheet->AppendNote(MusicalNote("Bb", octave + 1, MusicalNote::SIXTEENTH, bpm));
    sheet->AppendNote(MusicalNote("G", octave, MusicalNote::HALF, bpm));
}

void SheetMusicCatalog::LoadMarioTheme(SheetMusic *sheet, int octave, int bpm)
{
    sheet->Clear();

    sheet->AppendNote(MusicalNote("E", octave, MusicalNote::SIXTEENTH, bpm));
    sheet->AppendNote(MusicalNote("E", octave, MusicalNote::SIXTEENTH, bpm));
    sheet->AppendNote(MusicalNote("0", octave, MusicalNote::SIXTEENTH, bpm));
    sheet->AppendNote(MusicalNote("E", octave, MusicalNote::SIXTEENTH, bpm));
    sheet->AppendNote(MusicalNote("0", octave, MusicalNote::SIXTEENTH, bpm));
    sheet->AppendNote(MusicalNote("C", octave, MusicalNote::SIXTEENTH, bpm));
    sheet->AppendNote(MusicalNote("E", octave, MusicalNote::EIGHTH, bpm));
    sheet->AppendNote(MusicalNote("G", octave, MusicalNote::EIGHTH, bpm));
    sheet->AppendNote(MusicalNote("0", octave, MusicalNote::EIGHTH, bpm));
    sheet->AppendNote(MusicalNote("G", octave - 1, MusicalNote::EIGHTH, bpm));
    sheet->AppendNote(MusicalNote("0", octave, MusicalNote::EIGHTH, bpm));
    // --
    sheet->AppendNote(MusicalNote("C", octave, 3 * MusicalNote::SIXTEENTH, bpm));
    sheet->AppendNote(MusicalNote("G", octave - 1, 3 * MusicalNote::SIXTEENTH, bpm));
    sheet->AppendNote(MusicalNote("E", octave - 1, MusicalNote::EIGHTH, bpm));
    sheet->AppendNote(MusicalNote("0", octave, MusicalNote::SIXTEENTH, bpm));
    sheet->AppendNote(MusicalNote("A", octave - 1, MusicalNote::EIGHTH, bpm));
    sheet->AppendNote(MusicalNote("B", octave - 1, 2 * MusicalNote::SIXTEENTH, bpm));
    sheet->AppendNote(MusicalNote("Bb", octave - 1, MusicalNote::SIXTEENTH, bpm));
    sheet->AppendNote(MusicalNote("A", octave - 1, MusicalNote::EIGHTH, bpm));
    // --
    sheet->AppendNote(MusicalNote("G", octave - 1, MusicalNote::QUARTER / 3, bpm));
    sheet->AppendNote(MusicalNote("E", octave, MusicalNote::QUARTER / 3, bpm));
    sheet->AppendNote(MusicalNote("G", octave, MusicalNote::QUARTER / 3, bpm));
    sheet->AppendNote(MusicalNote("A", octave, MusicalNote::EIGHTH, bpm));
    sheet->AppendNote(MusicalNote("F", octave, MusicalNote::SIXTEENTH, bpm));
    sheet->AppendNote(MusicalNote("G", octave, 2 * MusicalNote::SIXTEENTH, bpm));
    sheet->AppendNote(MusicalNote("E", octave, MusicalNote::EIGHTH, bpm));
    sheet->AppendNote(MusicalNote("C", octave, MusicalNote::SIXTEENTH, bpm));
    sheet->AppendNote(MusicalNote("D", octave, MusicalNote::SIXTEENTH, bpm));
    sheet->AppendNote(MusicalNote("B", octave - 1, MusicalNote::EIGHTH, bpm));
    sheet->AppendNote(MusicalNote("0", octave - 1, MusicalNote::SIXTEENTH, bpm));
    // --
    sheet->AppendNote(MusicalNote("C", octave, 3 * MusicalNote::SIXTEENTH, bpm));
    sheet->AppendNote(MusicalNote("G", octave - 1, 3 * MusicalNote::SIXTEENTH, bpm));
    sheet->AppendNote(MusicalNote("E", octave - 1, MusicalNote::EIGHTH, bpm));
    sheet->AppendNote(MusicalNote("0", octave, MusicalNote::SIXTEENTH, bpm));
    sheet->AppendNote(MusicalNote("A", octave - 1, MusicalNote::EIGHTH, bpm));
    sheet->AppendNote(MusicalNote("B", octave - 1, 2 * MusicalNote::SIXTEENTH, bpm));
    sheet->AppendNote(MusicalNote("Bb", octave - 1, MusicalNote::SIXTEENTH, bpm));
    sheet->AppendNote(MusicalNote("A", octave - 1, MusicalNote::EIGHTH, bpm));
    // --
    sheet->AppendNote(MusicalNote("G", octave - 1, MusicalNote::QUARTER / 3, bpm));
    sheet->AppendNote(MusicalNote("E", octave, MusicalNote::QUARTER / 3, bpm));
    sheet->AppendNote(MusicalNote("G", octave, MusicalNote::QUARTER / 3, bpm));
    sheet->AppendNote(MusicalNote("A", octave, MusicalNote::EIGHTH, bpm));
    sheet->AppendNote(MusicalNote("F", octave, MusicalNote::SIXTEENTH, bpm));
    sheet->AppendNote(MusicalNote("G", octave, 2 * MusicalNote::SIXTEENTH, bpm));
    sheet->AppendNote(MusicalNote("E", octave, MusicalNote::EIGHTH, bpm));
    sheet->AppendNote(MusicalNote("C", octave, MusicalNote::SIXTEENTH, bpm));
    sheet->AppendNote(MusicalNote("D", octave, MusicalNote::SIXTEENTH, bpm));
    sheet->AppendNote(MusicalNote("B", octave - 1, MusicalNote::EIGHTH, bpm));
    sheet->AppendNote(MusicalNote("0", octave - 1, MusicalNote::SIXTEENTH, bpm));
}
