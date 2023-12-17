#include "signal_processing.h"

// Variables needed for the Moving Average Filter
const int numReadings = 10;
int readings[numReadings];
int readIndex = 0;
float total = 0;

// Variables needed for the Low-Pass Filter
float lastLowPassFiltered = 0;
float alpha = 0.1; // Smoothing factor for low and high-pass filter

// Variables needed for the High-Pass Filter
float lastHighPassFiltered = 0;
float lastInput = 0;

float movingAverage(int newValue) {
    total = total - readings[readIndex];
    readings[readIndex] = newValue;
    total = total + readings[readIndex];
    readIndex = (readIndex + 1) % numReadings;

    return total / numReadings;
}

float lowPassFilter(int newValue) {
    float filteredValue = alpha * newValue + (1 - alpha) * lastLowPassFiltered;
    lastLowPassFiltered = filteredValue;
    return filteredValue;
}

float highPassFilter(int newValue) {
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

float notchFilter(int newValue) {
    notchOutput = newValue - 2 * cos(2 * M_PI * notchFrequency) * notchOutputPrevious + r * r * notchInputPrevious;
    notchInputPrevious = newValue;
    notchOutputPrevious = notchOutput;

    return k * notchOutput;
}


const int medianWindowSize = 5; // Window size for median filter
int medianWindow[medianWindowSize];
int medianIndex = 0;

float medianFilter(int newValue) {
    medianWindow[medianIndex] = newValue;
    medianIndex = (medianIndex + 1) % medianWindowSize;

    // Create a copy of the window for sorting
    int sortedWindow[medianWindowSize];
    for (int i = 0; i < medianWindowSize; i++) {
        sortedWindow[i] = medianWindow[i];
    }

    // Simple insertion sort
    for (int i = 0; i < medianWindowSize; i++) {
        for (int j = i; j > 0 && sortedWindow[j - 1] > sortedWindow[j]; j--) {
            int temp = sortedWindow[j];
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