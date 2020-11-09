#include "MusicalNote.h"

#define isequal(x, y) (strcmp(x, y) == 0)
#define isequal2(x, y1, y2) (strcmp(x, y1) == 0 || strcmp(x, y2) == 0)

double MusicalNote::GetNoteFreq(const char *note, int octave)
{
	double freq = 0;

	if (isequal(note, "C"))
		freq = note_C0;
	else if (isequal2(note, "C#", "Db"))
		freq = note_Db0;
	else if (isequal(note, "D"))
		freq = note_D0;
	else if (isequal2(note, "D#", "Eb"))
		freq = note_Eb0;
	else if (isequal(note, "E"))
		freq = note_E0;
	else if (isequal(note, "F"))
		freq = note_F0;
	else if (isequal2(note, "F#", "Gb"))
		freq = note_Gb0;
	else if (isequal(note, "G"))
		freq = note_G0;
	else if (isequal2(note, "G#", "Ab"))
		freq = note_Ab0;
	else if (isequal(note, "A"))
		freq = note_A0;
	else if (isequal2(note, "A#", "Bb"))
		freq = note_Bb0;
	else if (isequal(note, "B"))
		freq = note_B0;

	return freq * pow(2, octave);
}

double MusicalNote::GetNoteRpm(const char *note, int octave)
{
	double freq = GetNoteFreq(note, octave);
	double rpm = (freq * 60);
	return rpm;
}

double MusicalNote::ConvertNoteLengthToTime(double note_length, double bpm)
{
	double time_sec = 0;
	time_sec = ((60 * 4) / bpm) * note_length;
	return time_sec;
}
