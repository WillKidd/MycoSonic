#ifndef SIGNAL_PROCESSING_H
#define SIGNAL_PROCESSING_H

#include <Arduino.h>

float movingAverage(int newValue);
float lowPassFilter(int newValue);
float highPassFilter(int newValue);
float notchFilter(int newValue);
float medianFilter(int newValue);
float kalmanFilter(float newValue);
float applyFilter(int newValue, int filterIndex);

#endif