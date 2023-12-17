// signal_processing.h
#ifndef SignalProcessing_h
#define SignalProcessing_h

#include <Arduino.h>

float movingAverage(int newValue);
float lowPassFilter(int newValue);
float highPassFilter(int newValue);
float notchFilter(int newValue);
float medianFilter(int newValue);
float kalmanFilter(float newValue);

#endif
