#include "audio-math.h"

uint16_t midiNoteToFrequency(uint8_t midiNumber) {
    // Bounds checking
    if (midiNumber < MIN_MIDI_NOTE) midiNumber = MIN_MIDI_NOTE;
    if (midiNumber > MAX_MIDI_NOTE) midiNumber = MAX_MIDI_NOTE;

    // Calculate frequency in float
    float frequency = A4_FREQ * pow(SEMITONE_RATIO, midiNumber - MIDI_NOTE_A4);

    // Scale frequency to fit in uint16_t
    float maxFrequency = A4_FREQ * pow(SEMITONE_RATIO, MAX_MIDI_NOTE - MIDI_NOTE_A4);
    float scalingFactor = maxFrequency / 65535;
    uint16_t scaledFrequency = (uint16_t)(frequency / scalingFactor);

    return scaledFrequency;
}

// Function to calculate MIDI note number from frequency
uint8_t frequencyToMIDINote(float frequency) {
    // Handle non-positive frequencies by returning the lowest possible MIDI note
    if (frequency <= 0) return MIN_MIDI_NOTE;

    // Calculate the MIDI note number using log (natural logarithm)
    uint8_t midiNote = round(MIDI_NOTE_A4 + (12 * log(frequency / A4_FREQ) / log(2)));

    // Clamp the MIDI note to the valid range
    if (midiNote < MIN_MIDI_NOTE) midiNote = MIN_MIDI_NOTE;
    if (midiNote > MAX_MIDI_NOTE) midiNote = MAX_MIDI_NOTE;

    return midiNote;
}

// Function to map input to a scale in a certain key
uint16_t mapToScale(uint16_t input, uint16_t inputMin, uint16_t inputMax, uint8_t keyRootMidiNote, const uint8_t intervals[], uint8_t numIntervals) {
    uint8_t totalRange = MAX_MIDI_NOTE - MIN_MIDI_NOTE + 1;
    uint16_t totalNotesInScale = (totalRange / numIntervals) * numIntervals;

    uint16_t scaledInput = map(input, inputMin, inputMax, 0, totalNotesInScale - 1);

    uint8_t midiNote = keyRootMidiNote;
    for (uint16_t i = 0; i <= scaledInput; ++i) {
        midiNote += intervals[i % numIntervals];
        if (midiNote > MAX_MIDI_NOTE) {
            midiNote = MIN_MIDI_NOTE + (midiNote - MAX_MIDI_NOTE - 1) % totalRange;
        }
    }
    
    return midiNoteToFrequency(midiNote);
}

/*
uint16_t
uint8_t
*/
// Function for full spectrum mapping
uint16_t mapToFullSpectrum(uint16_t input, uint16_t inputMin, uint16_t inputMax) {
    uint8_t midiNumber = map(input, inputMin, inputMax, MIN_MIDI_NOTE, MAX_MIDI_NOTE);
    return midiNoteToFrequency(midiNumber);
}

// Function for dynamic range compression mapping
float dynamicRangeCompressionMapping(uint16_t input, uint16_t inputMin, uint16_t inputMax, uint8_t keyRootMidiNote, const uint8_t intervals[], uint8_t numIntervals) {
    // Apply compression to the input
    float compressedInput = log10(input - inputMin + 1) / log10(inputMax - inputMin + 1) * (inputMax - inputMin);

    // Now map this compressed input to a scale
    return mapToScale(compressedInput, inputMin, inputMax, keyRootMidiNote, intervals, numIntervals);
}

// Function for harmonic mapping
uint16_t harmonicMapping(uint16_t input, uint16_t inputMin, uint16_t inputMax, uint16_t baseFrequency) {
    // Normalize input to a harmonic index (1st, 2nd, 3rd harmonic, etc.)
    uint8_t harmonicIndex = map(input, inputMin, inputMax, 1, 10);

    // Calculate the frequency of the specified harmonic
    // Since baseFrequency is already scaled, the harmonic frequency must be calculated accordingly
    uint32_t harmonicFrequency = (uint32_t)baseFrequency * harmonicIndex;
    
    // Ensure the result fits within the uint16_t range
    if (harmonicFrequency > 65535) {
        harmonicFrequency = 65535;
    }
    return (uint16_t)harmonicFrequency;
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

float millisecondsPerBeat(uint8_t bpm) {
  return 60000 / float(bpm);
}

// Function to convert a note duration to time (milliseconds)
// noteType: The type of note (1 = whole note, 2 = half note, 4 = quarter note, etc.)
// bpm: Beats per minute
// beatUnit: The note value that represents one beat (bottom number in time signature)
// defaultBeatUnit: The default note value for one beat (usually a quarter note, represented as 4)

float noteDurationToTime(uint8_t noteType, uint8_t bpm, uint8_t beatUnit, uint8_t defaultBeatUnit = 4) {
    if (noteType <= 0){
        return 0.0;
    }
    float beatDuration = millisecondsPerBeat(bpm);

    // Adjust the duration based on the beat unit
    if (beatUnit != defaultBeatUnit) {
        beatDuration *= (float(defaultBeatUnit) / beatUnit);
    }

    // Calculate the duration of the specified note type
    return beatDuration * (4.0 / noteType);
}

float noteDurationToTimeFractional(float noteTypeRatio, uint8_t bpm, uint8_t beatUnit, uint8_t defaultBeatUnit = 4) {
    float beatDuration = millisecondsPerBeat(bpm);

    // Adjust the duration based on the beat unit
    if (beatUnit != defaultBeatUnit) {
        beatDuration *= (float(defaultBeatUnit) / beatUnit);
    }

    // Calculate the duration of the specified note type
    return beatDuration * noteTypeRatio;
}


