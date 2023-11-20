#include <MIDI.h>

// Create and bind the MIDI interface to the default hardware Serial port
MIDI_CREATE_DEFAULT_INSTANCE();

//unsigned long t1 = millis();

void handleNoteOn(byte inNoteNumber, byte inVelocity, byte inChannel)
{
  Serial.print("NoteOn  ");
  Serial.print(inNoteNumber);
  Serial.print("\tvelocity: ");
  Serial.println(inVelocity);
}

void handleNoteOff(byte inNoteNumber, byte inVelocity, byte inChannel)
{
  Serial.print("NoteOff ");
  Serial.print(inNoteNumber);
  Serial.print("\tvelocity: ");
  Serial.println(inVelocity);
}

void setup()
{
  //while (!Serial);

  MIDI.begin();
  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);
  
  Serial.begin(115200);
  Serial.println("Arduino ready.");
}

// -----------------------------------------------------------------------------
// MIDI Structor
//[8bytes][8bytes][8bytes]
//[Status][Data1][Data2]
//[0-15 type; 1-16 channel;][0-127 note;][0-127 pressure;]
// -----------------------------------------------------------------------------
void loop()
{
  MIDI.read();


  MIDI.sendNoteOn(20, 55, 1);
  delay(200);
  MIDI.sendNoteOn(40, 55, 1);
  delay(200);
  MIDI.sendNoteOn(80, 55, 1);
  delay(200);
  MIDI.sendNoteOn(20, 55, 1);
  delay(200);
  MIDI.sendNoteOn(40, 55, 1);
  delay(200);
  MIDI.sendNoteOn(80, 55, 1);
  delay(1000);
  MIDI.sendNoteOff(80, 55, 1);
  MIDI.sendNoteOff(40, 55, 1);
  MIDI.sendNoteOff(20, 55, 1);
}