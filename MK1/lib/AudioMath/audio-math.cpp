#include "audio-math.h"

#define A4_FREQ 440.0
#define SEMITONE_RATIO 1.059463094359 // 2^(1/12)
#define MIDI_NOTE_A4 69
#define MIN_MIDI_NOTE 21  // A0, lowest note on standard piano
#define MAX_MIDI_NOTE 108 // C8, highest note on standard piano
#define MAX_SCALE_NOTES 100 // Estimate for max number of notes needed

float midiNoteToFrequency(int midiNumber) {
    // Bounds checking
    if (midiNumber < MIN_MIDI_NOTE) midiNumber = MIN_MIDI_NOTE;
    if (midiNumber > MAX_MIDI_NOTE) midiNumber = MAX_MIDI_NOTE;

    // Calculate frequency
    return A4_FREQ * pow(SEMITONE_RATIO, midiNumber - MIDI_NOTE_A4);
}

// Function to calculate MIDI note number from frequency
int frequencyToMIDINote(float frequency) {
    // Handle non-positive frequencies by returning the lowest possible MIDI note
    if (frequency <= 0) return MIN_MIDI_NOTE;

    // Calculate the MIDI note number using log (natural logarithm)
    int midiNote = round(MIDI_NOTE_A4 + (12 * log(frequency / A4_FREQ) / log(2)));

    // Clamp the MIDI note to the valid range
    if (midiNote < MIN_MIDI_NOTE) midiNote = MIN_MIDI_NOTE;
    if (midiNote > MAX_MIDI_NOTE) midiNote = MAX_MIDI_NOTE;

    return midiNote;
}

// Function to map input to a scale in a certain key
float mapToScale(int input, int inputMin, int inputMax, int keyRootMidiNote, const int intervals[], int numIntervals) {
    int totalRange = MAX_MIDI_NOTE - MIN_MIDI_NOTE + 1;
    int totalNotesInScale = (totalRange / numIntervals) * numIntervals;

    int scaledInput = map(input, inputMin, inputMax, 0, totalNotesInScale - 1);

    int midiNote = keyRootMidiNote;
    for (int i = 0; i <= scaledInput; ++i) {
        midiNote += intervals[i % numIntervals];
        if (midiNote > MAX_MIDI_NOTE) {
            midiNote = MIN_MIDI_NOTE + (midiNote - MAX_MIDI_NOTE - 1) % totalRange;
        }
    }
    
    return midiNoteToFrequency(midiNote);
}

// Function for full spectrum mapping
float mapToFullSpectrum(int input, int inputMin, int inputMax) {
    int midiNumber = map(input, inputMin, inputMax, MIN_MIDI_NOTE, MAX_MIDI_NOTE);
    return midiNoteToFrequency(midiNumber);
}

// Function for dynamic range compression mapping
float dynamicRangeCompressionMapping(int input, int inputMin, int inputMax, int keyRootMidiNote, const int intervals[], int numIntervals) {
    // Apply compression to the input
    float compressedInput = log10(input - inputMin + 1) / log10(inputMax - inputMin + 1) * (inputMax - inputMin);

    // Now map this compressed input to a scale
    return mapToScale(compressedInput, inputMin, inputMax, keyRootMidiNote, intervals, numIntervals);
}

// Function for harmonic mapping
float harmonicMapping(int input, int inputMin, int inputMax, float baseFrequency) {
    // Normalize input to a harmonic index (1st, 2nd, 3rd harmonic, etc.)
    int harmonicIndex = map(input, inputMin, inputMax, 1, 10); // Example range: 1st to 10th harmonic

    // Calculate the frequency of the specified harmonic
    return baseFrequency * harmonicIndex;
}

/*
// Some intervals:
int majorIntervals[] = {2, 2, 1, 2, 2, 2, 1};
int dorianMode[] = {2, 1, 2, 2, 2, 1, 2};
int mixolydianMode[] = {2, 2, 1, 2, 2, 1, 2};
int minorIntervals[] = {2, 1, 2, 2, 1, 2, 2};
int harmonicMinorIntervals[] = {2, 1, 2, 2, 1, 3, 1};
int melodicMinorIntervals[] = {2, 1, 2, 2, 2, 2, 1};
int pentatonicMajorIntervals[] = {2, 2, 3, 2, 3};
int pentatonicMinorIntervals[] = {3, 2, 2, 3, 2};
int bluesIntervals[] = {3, 2, 1, 1, 3, 2};
int wholeToneIntervals[] = {2, 2, 2, 2, 2, 2};
int chromaticIntervals[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
int lydianDominantIntervals[] = {2, 2, 2, 1, 2, 1, 2};
int halfWholeDiminishedIntervals[] = {1, 2, 1, 2, 1, 2, 1, 2};
int wholeHalfDiminishedIntervals[] = {2, 1, 2, 1, 2, 1, 2, 1};
int alteredIntervals[] = {1, 2, 1, 2, 2, 2, 2};
int bebopDominantIntervals[] = {2, 2, 1, 2, 2, 1, 1, 1};
*/

float millisecondsPerBeat(int bpm) {
  return 60000 / float(bpm);
}

// Function to convert a note duration to time (milliseconds)
// noteType: The type of note (1 = whole note, 2 = half note, 4 = quarter note, etc.)
// bpm: Beats per minute
// beatUnit: The note value that represents one beat (bottom number in time signature)
// defaultBeatUnit: The default note value for one beat (usually a quarter note, represented as 4)

float noteDurationToTime(int noteType, int bpm, int beatUnit, int defaultBeatUnit = 4) {
    float beatDuration = millisecondsPerBeat(bpm);

    // Adjust the duration based on the beat unit
    if (beatUnit != defaultBeatUnit) {
        beatDuration *= (float(defaultBeatUnit) / beatUnit);
    }

    // Calculate the duration of the specified note type
    return beatDuration * (4.0 / noteType);
}

float noteDurationToTimeFractional(float noteTypeRatio, int bpm, int beatUnit, int defaultBeatUnit = 4) {
    float beatDuration = millisecondsPerBeat(bpm);

    // Adjust the duration based on the beat unit
    if (beatUnit != defaultBeatUnit) {
        beatDuration *= (float(defaultBeatUnit) / beatUnit);
    }

    // Calculate the duration of the specified note type
    return beatDuration * noteTypeRatio;
}


