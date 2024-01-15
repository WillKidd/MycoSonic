#include <Arduino.h>
#include "signal_processing.h"
#include "data_handler.h"
//#include <SPI.h>
#include "mozzi-audio.h"
#include "audio-math.h"
#include "input_handler.h"
#include "menu_system.h"
#include <avr/pgmspace.h>

const uint8_t bioSensorPin = A0;

bool useSDCardInput = true;
bool useSensorInput = false;

//bool useInputFilter = true;
bool useMovingAverage = true;
bool useLowPass = false;
bool useHighPass = false;
bool useNotch = false;
bool useMedian = false;
bool useKalman = false;

bool useMapToScale = false;
bool useMapToFullSpectrum = true;
bool useDynamicRangeCompression = false;
bool useHarmonicMapping = false;

bool useSineWave = true;
bool useTriangleWave = false;
bool useSquareWave = false;
bool useSawToothWave = false;

//bool useEffects = false;
bool usePhaseModulation = false;
bool useTremolo = false;
bool useBitCrusher = false;
bool useRingModulator = false;
bool useFlanger = false;
bool usePitchShifter = false;
bool useDistortion = false;
bool usePan = false;
bool useVibrato = false;
bool useLeslie = false;

bool useAudioOutput = true;
bool useSDCardOutput = false;
bool useMidiOutput = false;

uint8_t filterIndex = 0;

uint16_t dataValue;
uint16_t bioValue;

unsigned long lastChangeTime;
bool isNotePlaying = true;
uint8_t bpm = 120;           // Example BPM
uint8_t beatUnit = 4;        // Assuming 4/4 time signature
uint8_t defaultBeatUnit = 4; // Default beat unit for the time signature

// User-settable note and break durations (as note types, e.g., 4 for quarter note)
uint8_t noteDurationType = 4;  // Quarter note
uint8_t breakDurationType = 4; // Quarter note

float noteDuration;  // Duration of a note in milliseconds
float breakDuration; // Duration of a break in milliseconds

uint8_t keyMidiNote = 69;
uint8_t scaleType = 0;

uint8_t majorIntervals[] = {2, 2, 1, 2, 2, 2, 1};
uint8_t dorianMode[] = {2, 1, 2, 2, 2, 1, 2};
uint8_t mixolydianMode[] = {2, 2, 1, 2, 2, 1, 2};
uint8_t minorIntervals[] = {2, 1, 2, 2, 1, 2, 2};
uint8_t harmonicMinorIntervals[] = {2, 1, 2, 2, 1, 3, 1};
uint8_t melodicMinorIntervals[] = {2, 1, 2, 2, 2, 2, 1};
uint8_t pentatonicMajorIntervals[] = {2, 2, 3, 2, 3};
uint8_t pentatonicMinorIntervals[] = {3, 2, 2, 3, 2};
uint8_t bluesIntervals[] = {3, 2, 1, 1, 3, 2};
uint8_t wholeToneIntervals[] = {2, 2, 2, 2, 2, 2};
uint8_t chromaticIntervals[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
uint8_t lydianDominantIntervals[] = {2, 2, 2, 1, 2, 1, 2};
uint8_t halfWholeDiminishedIntervals[] = {1, 2, 1, 2, 1, 2, 1, 2};
uint8_t wholeHalfDiminishedIntervals[] = {2, 1, 2, 1, 2, 1, 2, 1};
uint8_t alteredIntervals[] = {1, 2, 1, 2, 2, 2, 2};
uint8_t bebopDominantIntervals[] = {2, 2, 1, 2, 2, 1, 1, 1};

// Array of pointers to the interval arrays
uint8_t* scaleIntervals[] = {
    majorIntervals,
    dorianMode,
    mixolydianMode,
    minorIntervals,
    harmonicMinorIntervals,
    melodicMinorIntervals,
    pentatonicMajorIntervals,
    pentatonicMinorIntervals,
    bluesIntervals,
    wholeToneIntervals,
    chromaticIntervals,
    lydianDominantIntervals,
    halfWholeDiminishedIntervals,
    wholeHalfDiminishedIntervals,
    alteredIntervals,
    bebopDominantIntervals
};

// TODO: find better parameter standard values and change return type of effects in mozzi_audio module
PhaseModulationEffect phaseModulationEffect(0.5f, 0.5f);
TremoloEffect tremoloEffect(0.5f, 0.5f);
BitCrusherEffect bitCrusherEffect(8, 8000);
RingModulatorEffect ringModulatorEffect(440);
FlangerEffect flangerEffect(10, 0.25f);
PitchShifterEffect pitchShifterEffect(1.5f);
DistortionEffect distortionEffect(2.0f, 0.5f);
PanEffect panEffect(0.5f);
VibratoEffect vibratoEffect(0.5f, 5.0f);
LeslieEffect leslieEffect(0.5f);

enum State
{
  PLAYING,
  BREAK
};
State currentState = PLAYING;

BaseWaveform *currentWaveform = nullptr;
uint16_t currentFrequency = 220;

const char rootItemName[] PROGMEM = "MycoSonic MK1";
const char inputItemName[] PROGMEM = "Input";
const char sensorItemName[] PROGMEM = "Sensor";
const char sdItemName[] PROGMEM = "SDCard";

const char inputFilterItemName[] PROGMEM = "Filters";
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
const char keyRootMidiNoteItemName[] PROGMEM = "KeyMidiNote";
const char scaleTypeItemName[] PROGMEM = "Scale";
const char mapToFullSpectrumItemName[] PROGMEM = "MapToFullSpec.";
const char dynamicRangeCompressionItemName[] PROGMEM = "DRC";
const char harmonicMappingItemName[] PROGMEM = "Harmonic";
const char baseFrequencyItemName[] PROGMEM = "BaseFrequency";

const char waveTypeItemName[] PROGMEM = "WaveType";
const char sineWaveItemName[] PROGMEM = "Sine";
const char triangleWaveItemName[] PROGMEM = "Triangle";
const char squareWaveItemName[] PROGMEM = "Square";
const char sawWaveItemName[] PROGMEM = "SawTooth";

const char outputFilterItemName[] PROGMEM = "Effects";
const char phaseModulationEffectItemName[] PROGMEM = "PhaseMod.";
const char tremoloEffectItemName[] PROGMEM = "Tremolo";
const char bitCrusherEffectItemName[] PROGMEM = "BitCrusher";
const char ringModulatorEffectItemName[] PROGMEM = "RingModulator";
const char flangerEffectItemName[] PROGMEM = "Flanger";
const char pitchShiferEffectItemName[] PROGMEM = "PitchShifter";
const char distortionEffectItemName[] PROGMEM = "Distortion";
const char panEffectItemName[] PROGMEM = "Pan";
const char vibratoEffectItemName[] PROGMEM = "Vibrato";
const char leslieEffectItemName[] PROGMEM = "Leslie";

const char outputItemName[] PROGMEM = "Output";
const char audioOutputItemName[] PROGMEM = "AudioOut";
const char sdCardOutputItemName[] PROGMEM = "SDCardOut";
const char midiOutputItemName[] PROGMEM = "MidiOut";

InputHandler inputHandler;
MenuItem rootItem(rootItemName, BASE_ITEM);

MenuItem inputItem(inputItemName, BASE_ITEM);
ToggleMenuItem sensorItem(sensorItemName, useSDCardInput, SINGLE_TOGGLE_ITEM);
ToggleMenuItem sdItem(sdItemName, useSensorInput, SINGLE_TOGGLE_ITEM);

MenuItem inputFilterItem(inputFilterItemName, BASE_ITEM);
ToggleMenuItem movingAverageItem(movingAverageItemName, useMovingAverage, SINGLE_TOGGLE_ITEM);
ToggleMenuItem lowPassItem(lowPassItemName, useLowPass, SINGLE_TOGGLE_ITEM);
ToggleMenuItem highPassItem(highPassItemName, useHighPass, SINGLE_TOGGLE_ITEM);
ToggleMenuItem notchItem(notchItemName, useNotch, SINGLE_TOGGLE_ITEM);
ToggleMenuItem medianItem(medianItemName, useMedian, SINGLE_TOGGLE_ITEM);
ToggleMenuItem kalmanItem(kalmanItemName, useKalman, SINGLE_TOGGLE_ITEM);

MenuItem timingItem(timingItemName, BASE_ITEM);
EditableMenuItemSingleUint8 bpmItem(BPMItemName, bpm);
EditableMenuItemSingleUint8 noteDurationItem(noteDurationItemName, noteDurationType);
EditableMenuItemSingleUint8 breakDurationItem(breakDurationItemName, breakDurationType);

MenuItem signalMappingItem(signalMappingItemName, BASE_ITEM);
ToggleMenuItem mapToScaleItem(mapToScaleItemName, useMapToScale, SINGLE_TOGGLE_ITEM);
EditableMenuItemSingleUint8 keyMidiNoteItem(keyRootMidiNoteItemName, keyMidiNote);
EditableMenuItemSingleUint8 scaleTypItem(scaleTypeItemName, scaleType);
ToggleMenuItem mapToFullSpectrumItem(mapToFullSpectrumItemName, useMapToFullSpectrum, SINGLE_TOGGLE_ITEM);
ToggleMenuItem dynamicRangeCompressionItem(dynamicRangeCompressionItemName, useDynamicRangeCompression, SINGLE_TOGGLE_ITEM);
ToggleMenuItem harmonicMappingItem(harmonicMappingItemName, useHarmonicMapping, SINGLE_TOGGLE_ITEM);

MenuItem waveTypeItem (waveTypeItemName, BASE_ITEM);
ToggleMenuItem sineWaveItem (sineWaveItemName, useSineWave, SINGLE_TOGGLE_ITEM);
ToggleMenuItem triangleWaveItem (triangleWaveItemName, useTriangleWave, SINGLE_TOGGLE_ITEM);
ToggleMenuItem squareWaveItem (squareWaveItemName, useSquareWave, SINGLE_TOGGLE_ITEM);
ToggleMenuItem sawWaveItem (sawWaveItemName, useSawToothWave, SINGLE_TOGGLE_ITEM);

MenuItem outputFilterItem(outputFilterItemName, BASE_ITEM);
ToggleMenuItem phaseModulationEffectItem(phaseModulationEffectItemName, usePhaseModulation, MULTI_TOGGLE_ITEM);
ToggleMenuItem tremoloEffectItem(tremoloEffectItemName, useTremolo, MULTI_TOGGLE_ITEM);
ToggleMenuItem bitCrusherEffectItem(bitCrusherEffectItemName, useBitCrusher, MULTI_TOGGLE_ITEM);
ToggleMenuItem ringModulatorEffectItem(ringModulatorEffectItemName, useRingModulator, MULTI_TOGGLE_ITEM);
ToggleMenuItem flangerEffectItem(flangerEffectItemName, useFlanger, MULTI_TOGGLE_ITEM);
ToggleMenuItem pitchShiferEffectItem(pitchShiferEffectItemName, usePitchShifter, MULTI_TOGGLE_ITEM);
ToggleMenuItem distortionEffectItem(distortionEffectItemName, useDistortion, MULTI_TOGGLE_ITEM);
ToggleMenuItem panEffectItem(panEffectItemName, usePan, MULTI_TOGGLE_ITEM);
ToggleMenuItem vibratoEffectItem(vibratoEffectItemName, useVibrato, MULTI_TOGGLE_ITEM);
ToggleMenuItem leslieEffectItem(leslieEffectItemName, useLeslie, MULTI_TOGGLE_ITEM);

MenuItem outputItem(outputItemName, BASE_ITEM);
ToggleMenuItem audioOutputItem(audioOutputItemName, useAudioOutput, SINGLE_TOGGLE_ITEM);
ToggleMenuItem sdCardOutputItem(sdCardOutputItemName, useSDCardOutput, SINGLE_TOGGLE_ITEM);
ToggleMenuItem midiOutputItem(midiOutputItemName, useMidiOutput, SINGLE_TOGGLE_ITEM);
MenuHandler menuHandler(&rootItem);

uint16_t applySelectedFilter(uint16_t input) {
    uint16_t filteredValue = input;

    if (useMovingAverage) {
        filteredValue = movingAverage(filteredValue);
    }
    else if (useLowPass) {
        filteredValue = lowPassFilter(filteredValue);
    }
    else if (useHighPass) {
        filteredValue = highPassFilter(filteredValue);
    }
    else if (useNotch) {
        filteredValue = notchFilter(filteredValue);
    }
    else if (useMedian) {
        filteredValue = medianFilter(filteredValue);
    }
    else if (useKalman) {
        filteredValue = kalmanFilter(filteredValue);
    }
    else {
      return input;
    }
    return filteredValue;
}

uint16_t applySelectedMapping(uint16_t input, uint16_t inputMin, uint16_t inputMax) {
    uint16_t mappedValue = input;

    if (useMapToScale) {
        // Ensure scaleType is within the range of defined scales
        scaleType = min(scaleType, sizeof(scaleIntervals) / sizeof(scaleIntervals[0]) - 1);
        uint8_t* intervals = scaleIntervals[scaleType];
        uint8_t numIntervals = sizeof(intervals) / sizeof(intervals[0]);

        mappedValue = mapToScale(mappedValue, inputMin, inputMax, keyMidiNote, intervals, numIntervals);
    }
    //DynamicRangeCompression not working yet
    if (useMapToFullSpectrum || useDynamicRangeCompression) {
        mappedValue = mapToFullSpectrum(mappedValue, inputMin, inputMax);
    }
    if (useHarmonicMapping) {
        uint16_t baseFrequency = A4_FREQ; // Example base frequency
        mappedValue = harmonicMapping(mappedValue, inputMin, inputMax, baseFrequency);
    }

    return mappedValue;
}

int applyEffectsChain(int inputSample) {
    // Enable or disable each effect based on the current boolean flags
    phaseModulationEffect.setEffectEnabled(usePhaseModulation);
    tremoloEffect.enableEffect(useTremolo);
    bitCrusherEffect.enableEffect(useBitCrusher);
    ringModulatorEffect.enableEffect(useRingModulator);
    flangerEffect.enableEffect(useFlanger);
    pitchShifterEffect.enableEffect(usePitchShifter);
    distortionEffect.enableEffect(useDistortion);
    panEffect.enableEffect(usePan);
    vibratoEffect.enableEffect(useVibrato);
    leslieEffect.enableEffect(useLeslie);

    // Apply each effect in sequence
    inputSample = phaseModulationEffect.applyEffect(inputSample);
    inputSample = tremoloEffect.applyEffect(inputSample);
    inputSample = bitCrusherEffect.applyEffect(inputSample);
    inputSample = ringModulatorEffect.applyEffect(inputSample);
    inputSample = flangerEffect.applyEffect(inputSample);
    inputSample = pitchShifterEffect.applyEffect(inputSample);
    inputSample = distortionEffect.applyEffect(inputSample);

    // For the Pan effect, handle left and right channels separately
    int leftChannel = inputSample;
    int rightChannel = inputSample;
    panEffect.applyEffect(leftChannel, rightChannel);
    inputSample = (leftChannel + rightChannel) / 2;  // Mix back to mono for simplicity

    inputSample = vibratoEffect.applyEffect(inputSample);
    inputSample = leslieEffect.applyEffect(inputSample);

    return inputSample;
}



void setup()
{
  Serial.begin(9600);
  initSDCard();
  startMozzi(64);

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
  timingItem.addChild(&bpmItem);
  timingItem.addChild(&noteDurationItem);
  timingItem.addChild(&breakDurationItem);

  rootItem.addChild(&signalMappingItem);
  signalMappingItem.addChild(&mapToScaleItem);
  mapToScaleItem.addChild(&keyMidiNoteItem);
  mapToScaleItem.addChild(&scaleTypItem);
  signalMappingItem.addChild(&mapToFullSpectrumItem);
  signalMappingItem.addChild(&dynamicRangeCompressionItem);
  signalMappingItem.addChild(&harmonicMappingItem);

  rootItem.addChild(&waveTypeItem);
  waveTypeItem.addChild(&sineWaveItem);
  waveTypeItem.addChild(&triangleWaveItem);
  waveTypeItem.addChild(&squareWaveItem);
  waveTypeItem.addChild(&sawWaveItem);

  rootItem.addChild(&outputFilterItem);
  outputFilterItem.addChild(&phaseModulationEffectItem);
  outputFilterItem.addChild(&tremoloEffectItem);
  outputFilterItem.addChild(&bitCrusherEffectItem);
  outputFilterItem.addChild(&ringModulatorEffectItem);
  outputFilterItem.addChild(&flangerEffectItem);
  outputFilterItem.addChild(&pitchShiferEffectItem);
  outputFilterItem.addChild(&distortionEffectItem);
  outputFilterItem.addChild(&panEffectItem);
  outputFilterItem.addChild(&vibratoEffectItem);
  outputFilterItem.addChild(&leslieEffectItem);

  rootItem.addChild(&outputItem);
  outputItem.addChild(&audioOutputItem);
  outputItem.addChild(&sdCardOutputItem);
  outputItem.addChild(&midiOutputItem);

  menuHandler.setLCDHandler(&lcdHandler);
  menuHandler.selectItem();
  menuHandler.displayCurrentItem();
  currentWaveform = new SineWave(0);
  currentWaveform->setFrequency(0);
  // Serial.println(inputItem.getName());
}

void loop()
{
  audioHook();
}

void updateControl()
{
  unsigned long currentTime = millis();

  inputHandler.update();
  if (inputHandler.isOkPressed())
  {
    // Serial.println("OK");
    if (menuHandler.isEditMode())
    {
      menuHandler.saveChanges();  // Save changes if in edit mode
      menuHandler.exitEditMode(); // Exit edit mode
    }
    // Serial.println(menuHandler.getCurrentItem()->getName());
    menuHandler.displayCurrentItem();
  }
else if (inputHandler.isEditPressed()) {
    MenuItem* currentItem = menuHandler.getCurrentItem();
    if (currentItem->getType() == EDITABLE_ITEM || currentItem->getType() == EDITABLE_UINT8_ITEM) {
        // If the current item is editable, enter edit mode
        menuHandler.enterEditMode();
    } else {
        // For non-editable items, just select the item
        menuHandler.selectItem();
    }
    //Serial.println(currentItem->getName());
    menuHandler.displayCurrentItem();
}

  else if (inputHandler.isBackPressed())
  {
    // Serial.println("BACK");
    if (menuHandler.isEditMode())
    {
      menuHandler.discardChanges(); // Discard changes if in edit mode
      menuHandler.exitEditMode();   // Exit edit mode
      menuHandler.back();
    }
    else
    {
      menuHandler.back(); // Navigate back in the menu
    }
    // Serial.println(menuHandler.getCurrentItem()->getName());
    menuHandler.displayCurrentItem();
  }
  else if (inputHandler.isToggleButtonPressed())
  {
    // Serial.println("TOGGLE");
    if (!menuHandler.isEditMode())
    {
      menuHandler.toggleCurrentItem(); // Toggle the current item
    }
    // Serial.println(menuHandler.getCurrentItem()->getName());
    menuHandler.displayCurrentItem();
  }
  else if (inputHandler.isUpButtonPressed())
  {
    if (menuHandler.isEditMode())
    {
      MenuItem *currentItem = menuHandler.getCurrentItem();
      switch (currentItem->getType())
      {
      case EDITABLE_ITEM:
        menuHandler.updateParameterValue(0.1);
        break;
      case EDITABLE_UINT8_ITEM:
        menuHandler.updateParameterValueUint8(1);
        break;
      }
    }
    else
    {
      menuHandler.previousItem();
    }
    // Serial.println(menuHandler.getCurrentItem()->getName());
    menuHandler.displayCurrentItem();
  }
  else if (inputHandler.isDownButtonPressed())
  {
    if (menuHandler.isEditMode())
    {
      MenuItem *currentItem = menuHandler.getCurrentItem();
      switch (currentItem->getType())
      {
      case EDITABLE_ITEM:
        menuHandler.updateParameterValue(-0.1);
        break;
      case EDITABLE_UINT8_ITEM:
        menuHandler.updateParameterValueUint8(-1);
        break;
      }
    }
    else
    {
      menuHandler.nextItem();
    }
    // Serial.println(menuHandler.getCurrentItem()->getName());
    menuHandler.displayCurrentItem();
  }

  switch (currentState)
  {
  case PLAYING:
    if (currentTime - lastChangeTime >= noteDuration)
    {
      currentWaveform->setFrequency(0); // Silence the output for a break
      currentState = BREAK;
      lastChangeTime = currentTime;
    }
    break;
  case BREAK:
    if (currentTime - lastChangeTime >= breakDuration)
    {
      if (useSensorInput){
        bioValue = analogRead(bioSensorPin);
      } 
      else if (useSDCardInput){
        bioValue = readDataFromSD();
      }
      if(useAudioOutput){
        noteDuration = noteDurationToTime(noteDurationType, bpm, beatUnit, defaultBeatUnit);
        breakDuration = noteDurationToTime(breakDurationType, bpm, beatUnit, defaultBeatUnit);
        dataValue = applySelectedFilter(bioValue);
        uint16_t freq = mapToFullSpectrum(dataValue, 0, 1023);
        currentWaveform->setFrequency(freq);
      } 
      else if (useSDCardOutput) {
        char buffer[4];
        snprintf(buffer, sizeof(buffer), "%u", bioValue);
        logData(String(buffer));
      }
      else if (useMidiOutput){

      }
      currentState = PLAYING;
      lastChangeTime = currentTime;
    }
    break;
  }
}

int updateAudio()
{
  if (useAudioOutput){
    int output = currentWaveform->update();
    return applyEffectsChain(output);
  }
}