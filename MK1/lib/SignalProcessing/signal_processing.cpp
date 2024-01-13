#include "signal_processing.h"

// Variables needed for the Moving Average Filter
const uint8_t numReadings = 10;
uint16_t readings[numReadings];
uint8_t readIndex = 0;
float total = 0;

// Variables needed for the Low-Pass Filter
float lastLowPassFiltered = 0;
float alpha = 0.1; // Smoothing factor for low and high-pass filter

// Variables needed for the High-Pass Filter
float lastHighPassFiltered = 0;
float lastInput = 0;

float movingAverage(uint16_t newValue) {
    total = total - readings[readIndex];
    readings[readIndex] = newValue;
    total = total + readings[readIndex];
    readIndex = (readIndex + 1) % numReadings;

    return total / numReadings;
}

float lowPassFilter(uint16_t newValue) {
    float filteredValue = alpha * newValue + (1 - alpha) * lastLowPassFiltered;
    lastLowPassFiltered = filteredValue;
    return filteredValue;
}

float highPassFilter(uint16_t newValue) {
    // Apply the low-pass filter to the new value
    float lowPass = lowPassFilter(newValue);
    // Calculate the high-pass filter output
    float highPass = newValue - lowPass + alpha * lastHighPassFiltered;
    lastHighPassFiltered = highPass;
    lastInput = newValue;
    return highPass;
}

// Variables needed for the Notch Filter
float notchOutput = 0;
float notchInputPrevious = 0;
float notchOutputPrevious = 0;
float notchFrequency = 0.1; // The frequency to be notched out, as a fraction of the sampling rate
float notchBandwidth = 0.05; // The bandwidth around the notch frequency, as a fraction of the sampling rate
float r = 1 - 3 * notchBandwidth;
float k = (1 - 2 * r * cos(2 * M_PI * notchFrequency) + r * r) / (2 - 2 * cos(2 * M_PI * notchFrequency));

float notchFilter(uint16_t newValue) {
    notchOutput = newValue - 2 * cos(2 * M_PI * notchFrequency) * notchOutputPrevious + r * r * notchInputPrevious;
    notchInputPrevious = newValue;
    notchOutputPrevious = notchOutput;

    return k * notchOutput;
}


const uint8_t medianWindowSize = 5; // Window size for median filter
uint16_t medianWindow[medianWindowSize];
uint8_t medianIndex = 0;

float medianFilter(int newValue) {
    medianWindow[medianIndex] = newValue;
    medianIndex = (medianIndex + 1) % medianWindowSize;

    // Create a copy of the window for sorting
    uint16_t sortedWindow[medianWindowSize];
    for (uint8_t i = 0; i < medianWindowSize; i++) {
        sortedWindow[i] = medianWindow[i];
    }

    // Simple insertion sort
    for (uint8_t i = 0; i < medianWindowSize; i++) {
        for (uint16_t j = i; j > 0 && sortedWindow[j - 1] > sortedWindow[j]; j--) {
            uint16_t temp = sortedWindow[j];
            sortedWindow[j] = sortedWindow[j - 1];
            sortedWindow[j - 1] = temp;
        }
    }

    // Return the median value
    return sortedWindow[medianWindowSize / 2];
}

// Variables for Kalman Filter
float kalmanEstimate = 0; // The current Kalman estimate
float kalmanErrorEstimate = 1; // Error in the Kalman estimate
float kalmanErrorMeasure = 1; // Measurement error
float kalmanQ = 0.1; // Process noise
float kalmanGain = 0;

float kalmanFilter(float newValue) {
    // Prediction Update
    // No prediction update needed for a simple 1D Kalman Filter

    // Measurement Update
    kalmanGain = kalmanErrorEstimate / (kalmanErrorEstimate + kalmanErrorMeasure);
    kalmanEstimate = kalmanEstimate + kalmanGain * (newValue - kalmanEstimate);
    kalmanErrorEstimate = (1 - kalmanGain) * kalmanErrorEstimate + fabs(kalmanQ - kalmanGain * kalmanErrorEstimate);

    return kalmanEstimate;
}


float applyFilter(uint16_t bioValue, uint8_t filterIndex) {
  float filteredValue;
  switch (filterIndex) {
    case 0:
      filteredValue = movingAverage(bioValue);
      break;
    case 1:
      filteredValue = lowPassFilter(bioValue);
      break;
    case 2:
      filteredValue = highPassFilter(bioValue);
      break;
    case 3:
      filteredValue = notchFilter(bioValue);
      break;
    case 4:
      filteredValue = medianFilter(bioValue);
      break;
    case 5:
      filteredValue = kalmanFilter(bioValue);
      break;
    default:
      filteredValue = bioValue;
      break;
  }
  return filteredValue;
}