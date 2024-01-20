#include "midi-output.h"


MIDI_CREATE_DEFAULT_INSTANCE();

MidiOutput::MidiOutput() {
}

void MidiOutput::begin() {
    MIDI.begin(MIDI_CHANNEL_OMNI);
}

void MidiOutput::sendNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
    MIDI.sendNoteOn(note, velocity, channel);
}

void MidiOutput::sendNoteOff(uint8_t channel, uint8_t note, uint8_t velocity) {
    MIDI.sendNoteOff(note, velocity, channel);
}

void MidiOutput::sendControlChange(uint8_t channel, uint8_t control, uint8_t value) {
  MIDI.sendControlChange(control, value, channel);
}
