#include <Arduino.h>
int speakerPin = 9; // Use the same pin as Mozzi
bool ouputEnabled = false;
int sMin=0;
int sMax=1023;
int fMin;
int fMax;

void changeSignalMapping(int frequencyMin, int frequencyMax, int sensorMin, int sensorMax){
    fMin = frequencyMin;
    fMax = frequencyMax;
    sMin = sensorMin;
    sMax = sensorMax;
}

void initAudioOutput(int frequencyMin, int frequencyMax, int sensorMin=0, int sensorMax=1023) {
    changeSignalMapping(frequencyMin, frequencyMax, sensorMin, sensorMax);
    ouputEnabled = true;
}

void playToneFor(){
    if (!ouputEnabled){
        return;
    }
}

void setup() {
  pinMode(speakerPin, OUTPUT);
}

void loop() {
  tone(speakerPin, 440); 
  delay(1000);           // for 1 second
  noTone(speakerPin);    // Stop the tone
  delay(1000);           // for 1 second
}

