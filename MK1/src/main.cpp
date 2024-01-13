#include <Arduino.h>
#include "signal_processing.h"
//#include "data_handler.h"
#include <SPI.h>
#include "mozzi-audio.h"
#include "audio-math.h"
#include "input_handler.h"
#include "menu_system.h"
#include <avr/pgmspace.h>

const uint8_t bioSensorPin = A0;

// in data_handler
//bool enableSDRead = false;
//bool enableSDWrite = false;
bool useSDCardData = false;
bool useSensorData = true;

bool useMovingAverage = true;
bool useLowPass = false;
bool useHighPass = false;
bool useNotch = false;
bool useMedian = false;
bool useKalman = false;
uint8_t filterIndex = 0;

bool logDataToSDCard = false;

float dataValue;
uint16_t bioValue;

unsigned long lastChangeTime;
bool isNotePlaying = true;
uint8_t bpm = 120; // Example BPM
uint8_t beatUnit = 4; // Assuming 4/4 time signature
uint8_t defaultBeatUnit = 4; // Default beat unit for the time signature

// User-settable note and break durations (as note types, e.g., 4 for quarter note)
uint8_t noteDurationType = 4; // Quarter note
uint8_t breakDurationType = 4; // Quarter note

float noteDuration; // Duration of a note in milliseconds
float breakDuration; // Duration of a break in milliseconds

enum State { PLAYING, BREAK };
State currentState = PLAYING;

BaseWaveform* currentWaveform = nullptr;
uint16_t currentFrequency = 220;

const char rootItemName[] PROGMEM = "MycoSonic MK1";
const char inputItemName[] PROGMEM = "Input";
const char sensorItemName[] PROGMEM = "Sensor";
const char sdItemName[] PROGMEM = "SDCardIn";
const char inputFilterItemName[] PROGMEM = "Filter";
const char directInputItemName[] PROGMEM = "DirectInput";
const char movingAverageItemName[] PROGMEM = "MovingAverage";
const char lowPassItemName[] PROGMEM = "LowPass";
const char highPassItemName[] PROGMEM = "HighPass";
const char notchItemName[] PROGMEM = "Notch";
const char medianItemName[] PROGMEM = "Median";
const char kalmanItemName[] PROGMEM = "Kalman";
const char timingItemName[] PROGMEM = "Timing";
const char BPMItemName[] PROGMEM = "BPM";
const char noteDurationItemName[] PROGMEM = "NoteLength";
const char breakDurationItemName[] PROGMEM = "BreakLength";
const char signalMappingItemName[] PROGMEM = "Mapping";
const char mapToScaleItemName[] PROGMEM = "MapToScale";
const char MapToFullSpectrum[] PROGMEM = "MapToFullSpectrum";
const char DynamicRangeCompression[] PROGMEM = "DynamicRangeCompression";
const char HarmonicMapping[] PROGMEM = "HarmonicMapping";
const char outputFilterItemName[] PROGMEM = "Effects";
const char phaseModulationEffectItemName[] PROGMEM = "PhaseMod.";
const char tremoloEffectItemName[] PROGMEM = "Tremolo";
const char bitCrusherEffectItemName[] PROGMEM = "BitCrusherEffect";
const char ringModulatorEffectItemName[] PROGMEM = "RingModulator";
const char flangerEffectItemName[] PROGMEM = "Flanger";
const char pitchShiferEffectItemName[] PROGMEM = "PitchShifter";
const char distortionEffectItemName[] PROGMEM = "Distortion";
const char panEffectItemName[] PROGMEM = "Pan";
const char vibratoEffectItemName[] PROGMEM = "Vibrato";
const char leslieEffectItemName[] PROGMEM = "Leslie";
const char outputItemName[] PROGMEM = "Output";
const char audioOutputItemName[] PROGMEM = "AudioOut";
const char sdcardOutputItemName[] PROGMEM = "SDCardOut";
const char midiOutputItemName[] PROGMEM = "MidiOut";

InputHandler inputHandler;
MenuItem rootItem(rootItemName, BASE_ITEM);
MenuItem inputItem(inputItemName, BASE_ITEM);
ToggleMenuItem sensorItem(sensorItemName, useSensorData, SINGLE_TOGGLE_ITEM);
ToggleMenuItem sdItem(sdItemName, useSDCardData, SINGLE_TOGGLE_ITEM);
MenuItem inputFilterItem(inputFilterItemName, BASE_ITEM);
ToggleMenuItem directInputItem(directInputItemName, useMovingAverage, SINGLE_TOGGLE_ITEM);
ToggleMenuItem movingAverageItem(movingAverageItemName, useMovingAverage, SINGLE_TOGGLE_ITEM);
ToggleMenuItem lowPassItem(lowPassItemName, useLowPass, SINGLE_TOGGLE_ITEM);
ToggleMenuItem highPassItem(highPassItemName, useHighPass, SINGLE_TOGGLE_ITEM);
ToggleMenuItem notchItem(notchItemName, useNotch, SINGLE_TOGGLE_ITEM);
ToggleMenuItem medianItem(medianItemName, useMedian, SINGLE_TOGGLE_ITEM);
ToggleMenuItem kalmanItem(kalmanItemName, useKalman, SINGLE_TOGGLE_ITEM);
MenuItem timingItem(timingItemName, BASE_ITEM);
MenuItem signalMappingItem(signalMappingItemName, BASE_ITEM); 
MenuItem outputFilterItem(outputFilterItemName, BASE_ITEM);
MenuItem outputItem(outputItemName, BASE_ITEM);
MenuHandler menuHandler(&rootItem);

void setup() {
  Serial.begin(9600);
  //initSDCard();
  startMozzi(64);

  noteDuration = noteDurationToTime(noteDurationType, bpm, beatUnit, defaultBeatUnit);
  breakDuration = noteDurationToTime(breakDurationType, bpm, beatUnit, defaultBeatUnit);

  currentWaveform = new SawWave(currentFrequency);
  lastChangeTime = millis();
  LCDHandler lcdHandler(0x27, 16, 2);
  lcdHandler.init();
  
  rootItem.addChild(&inputItem);
  inputItem.addChild(&sensorItem);
  inputItem.addChild(&sdItem);
  rootItem.addChild(&inputFilterItem);
  inputFilterItem.addChild(&movingAverageItem);
  inputFilterItem.addChild(&lowPassItem);
  inputFilterItem.addChild(&highPassItem);
  inputFilterItem.addChild(&notchItem);
  inputFilterItem.addChild(&medianItem);
  inputFilterItem.addChild(&kalmanItem);
  rootItem.addChild(&timingItem);
  rootItem.addChild(&signalMappingItem); 
  rootItem.addChild(&outputFilterItem);
  rootItem.addChild(&outputItem);
  menuHandler.setLCDHandler(&lcdHandler);
  menuHandler.selectItem();
  menuHandler.displayCurrentItem();
  //Serial.println(inputItem.getName());
}

void loop(){
  audioHook();
}


void updateControl(){
  unsigned long currentTime = millis();
  
  inputHandler.update();
  if (inputHandler.isOkPressed()) {
    //Serial.println("OK");
    if (menuHandler.isEditMode()) {
        menuHandler.saveChanges(); // Save changes if in edit mode
        menuHandler.exitEditMode(); // Exit edit mode
    }
    //Serial.println(menuHandler.getCurrentItem()->getName());
    menuHandler.displayCurrentItem();
  }
  else if (inputHandler.isEditPressed()) {
      //Serial.println("EDIT");
      menuHandler.enterEditMode(); // Enter edit mode
      menuHandler.selectItem();
      //Serial.println(menuHandler.getCurrentItem()->getName());
      menuHandler.displayCurrentItem();
  }
  else if (inputHandler.isBackPressed()) {
      //Serial.println("BACK");
    if (menuHandler.isEditMode()) {
        menuHandler.discardChanges(); // Discard changes if in edit mode
        menuHandler.exitEditMode(); // Exit edit mode
        menuHandler.back();
    } else {
        menuHandler.back(); // Navigate back in the menu
    }
    //Serial.println(menuHandler.getCurrentItem()->getName());
    menuHandler.displayCurrentItem();
  }
  else if (inputHandler.isToggleButtonPressed()) {
    //Serial.println("TOGGLE");
    if (!menuHandler.isEditMode()){
      menuHandler.toggleCurrentItem(); // Toggle the current item
    }
    //Serial.println(menuHandler.getCurrentItem()->getName());
    menuHandler.displayCurrentItem();
  }
 else if (inputHandler.isUpButtonPressed()) {
    if (menuHandler.isEditMode()) {
      menuHandler.updateParameterValue(0.1);
    } 
    else {
      menuHandler.previousItem();
    }
    //Serial.println(menuHandler.getCurrentItem()->getName());
    menuHandler.displayCurrentItem();
  }
  else if (inputHandler.isDownButtonPressed()) {
    if (menuHandler.isEditMode()) {
      menuHandler.updateParameterValue(-0.1);
    } 
    else {
      menuHandler.nextItem();
    }
    //Serial.println(menuHandler.getCurrentItem()->getName());
    menuHandler.displayCurrentItem();
  }
  
  switch (currentState) {
    case PLAYING:
      if (currentTime - lastChangeTime >= noteDuration) {
        currentWaveform->setFrequency(0); // Silence the output for a break
        currentState = BREAK;
        lastChangeTime = currentTime;
      }
      break;
    case BREAK:
      if (currentTime - lastChangeTime >= breakDuration) {
        bioValue = analogRead(bioSensorPin);
        dataValue = applyFilter(bioValue, filterIndex);
        float freq = harmonicMapping(dataValue, 0, 1023, 110);
        //currentWaveform->setFrequency(freq);
        currentWaveform->setFrequency(110); //for testing not as annoying
        currentState = PLAYING;
        lastChangeTime = currentTime;
      }
      break;
  }
}

int updateAudio(){
  return currentWaveform->update();
}