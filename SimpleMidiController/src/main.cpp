#include <MIDI.h>

// Create and bind the MIDI interface to the default hardware Serial port
MIDI_CREATE_DEFAULT_INSTANCE();

unsigned long t1 = millis();

void handleNoteOn(byte inChannel, byte inNumber, byte inVelocity)
{
  Serial.print("NoteOn  ");
  Serial.print(inNumber);
  Serial.print("\tvelocity: ");
  Serial.println(inVelocity);
}

void handleNoteOff(byte inChannel, byte inNumber, byte inVelocity)
{
  Serial.print("NoteOff ");
  Serial.print(inNumber);
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
//
// -----------------------------------------------------------------------------
void loop()
{
  MIDI.read();

  if ((millis() - t1) > 500)
  {
    t1 = millis();

    MIDI.sendNoteOn(27, 55, 1);
    MIDI.sendNoteOff(27, 55, 1);
  }
}