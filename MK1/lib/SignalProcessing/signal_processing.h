#ifndef SIGNAL_PROCESSING_H
#define SIGNAL_PROCESSING_H

#include <Arduino.h>

float movingAverage(uint16_t newValue);
float lowPassFilter(uint16_t newValue);
float highPassFilter(uint16_t newValue);
float notchFilter(uint16_t newValue);
float medianFilter(uint16_t newValue);
float kalmanFilter(float newValue);
float applyFilter(uint16_t newValue, uint8_t filterIndex);

#endif
