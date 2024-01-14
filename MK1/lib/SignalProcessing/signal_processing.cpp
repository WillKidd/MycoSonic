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

uint16_t movingAverage(uint16_t newValue) {
    total = total - readings[readIndex];
    readings[readIndex] = newValue;
    total = total + readings[readIndex];
    readIndex = (readIndex + 1) % numReadings;

    return (uint16_t)(total / numReadings + 0.5); // Round to nearest integer
}

uint16_t lowPassFilter(uint16_t newValue) {
    float filteredValue = alpha * newValue + (1 - alpha) * lastLowPassFiltered;
    lastLowPassFiltered = filteredValue;
    return (uint16_t)(filteredValue + 0.5);
}

uint16_t highPassFilter(uint16_t newValue) {
    float lowPass = lowPassFilter(newValue);
    float highPass = newValue - lowPass + alpha * lastHighPassFiltered;
    lastHighPassFiltered = highPass;
    lastInput = newValue;
    return (uint16_t)(highPass + 0.5);
}

// Variables needed for the Notch Filter
float notchOutput = 0;
float notchInputPrevious = 0;
float notchOutputPrevious = 0;
float notchFrequency = 0.1; // The frequency to be notched out, as a fraction of the sampling rate
float notchBandwidth = 0.05; // The bandwidth around the notch frequency, as a fraction of the sampling rate
float r = 1 - 3 * notchBandwidth;
float k = (1 - 2 * r * cos(2 * M_PI * notchFrequency) + r * r) / (2 - 2 * cos(2 * M_PI * notchFrequency));

uint16_t notchFilter(uint16_t newValue) {
    notchOutput = newValue - 2 * cos(2 * M_PI * notchFrequency) * notchOutputPrevious + r * r * notchInputPrevious;
    notchInputPrevious = newValue;
    notchOutputPrevious = notchOutput;

    return (uint16_t)(k * notchOutput + 0.5);
}


const uint8_t medianWindowSize = 5; // Window size for median filter
uint16_t medianWindow[medianWindowSize];
uint8_t medianIndex = 0;

uint16_t medianFilter(uint16_t newValue) {
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

uint16_t kalmanFilter(uint16_t newValue) {
    kalmanGain = kalmanErrorEstimate / (kalmanErrorEstimate + kalmanErrorMeasure);
    kalmanEstimate = kalmanEstimate + kalmanGain * (newValue - kalmanEstimate);
    kalmanErrorEstimate = (1 - kalmanGain) * kalmanErrorEstimate + fabs(kalmanQ - kalmanGain * kalmanErrorEstimate);

    return (uint16_t)(kalmanEstimate + 0.5);
}

