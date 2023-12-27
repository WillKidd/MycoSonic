#ifndef AUDIO_MATH_H
#define AUDIO_MATH_H

#include <Arduino.h>
#include <math.h>

// Constants
#define A4_FREQ 440.0
#define SEMITONE_RATIO 1.059463094359 // 2^(1/12)
#define MIDI_NOTE_A4 69
#define MIN_MIDI_NOTE 21  // A0, lowest note on standard piano
#define MAX_MIDI_NOTE 108 // C8, highest note on standard piano
#define MAX_SCALE_NOTES 100 // Estimate for max number of notes needed

// Enum to specify the mapping type
enum MappingType {
    MAP_TO_SCALE,
    MAP_TO_FULL_SPECTRUM
};

// Function Declarations
float midiNoteToFrequency(int midiNumber);
int frequencyToMIDINote(float frequency);
float mapInputToFrequency(int input, int inputMin, int inputMax, const int intervals[], int numIntervals, MappingType mappingType, float baseFrequency);
float bpmToMilliseconds(int bpm);
float noteDurationToTime(int noteType, int bpm, int beatUnit, int defaultBeatUnit);
#endif
