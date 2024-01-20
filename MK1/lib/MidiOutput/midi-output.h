#ifndef MIDI_OUTPUT_H
#define MIDI_OUTPUT_H

#include <MIDI.h>

class MidiOutput {
public:
    MidiOutput();
    void begin();
    void sendNoteOn(uint8_t channel, uint8_t note, uint8_t velocity);
    void sendNoteOff(uint8_t channel, uint8_t note, uint8_t velocity);
    void sendControlChange(uint8_t channel, uint8_t control, uint8_t value);
   
};

#endif
