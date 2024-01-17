#ifndef AUDIO_MATH_H
#define AUDIO_MATH_H

#include <Arduino.h>
#include <math.h>

// Constants
const uint16_t A4_FREQ = 440;
const float SEMITONE_RATIO = 1.059463094359; // 2^(1/12)
const uint8_t MIDI_NOTE_A4 = 69;
const uint8_t MIN_MIDI_NOTE = 21;  // A0, lowest note on standard piano
const uint8_t MAX_MIDI_NOTE = 108; // C8, highest note on standard piano
const uint8_t MAX_SCALE_NOTES = 100; // Estimate for max number of notes needed

// Enum to specify the mapping type
enum MappingType {
    MAP_TO_FULL_SPECTRUM,
    MAP_TO_SCALE,
    DYNAMIC_RANGE_COMPRESSION,
    HARMONIC_MAPPING
};

// Function Declarations
//uint16_t midiNoteToFrequency(uint8_t midiNumber);
float midiNoteToFrequency(int midiNumber);
int frequencyToMIDINote(float frequency);
float mapToScale(int input, int inputMin, int inputMax, int keyRootMidiNote, const int intervals[], int numIntervals);
float mapToFullSpectrum(int input, int inputMin, int inputMax);
float dynamicRangeCompressionMapping(int input, int inputMin, int inputMax, const int intervals[], int numIntervals);
float harmonicMapping(int input, int inputMin, int inputMax, float baseFrequency);
float mapInputToFrequency(int input, int inputMin, int inputMax, const int intervals[], int numIntervals, MappingType mappingType, float baseFrequency);
float millisecondsPerBeat(int bpm);
float noteDurationToTime(int noteType, int bpm, int beatUnit, int defaultBeatUnit);
float noteDurationToTimeFractional(float noteTypeRatio, int bpm, int beatUnit, int defaultBeatUnit);
#endif
