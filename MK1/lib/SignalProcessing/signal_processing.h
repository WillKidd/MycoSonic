#ifndef SIGNAL_PROCESSING_H
#define SIGNAL_PROCESSING_H

#include <Arduino.h>

uint16_t movingAverage(uint16_t newValue);
uint16_t lowPassFilter(uint16_t newValue);
uint16_t highPassFilter(uint16_t newValue);
uint16_t notchFilter(uint16_t newValue);
uint16_t medianFilter(uint16_t newValue);
uint16_t kalmanFilter(uint16_t newValue);

#endif
