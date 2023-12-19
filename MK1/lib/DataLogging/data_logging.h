#ifndef DATA_LOGGING_H
#define DATA_LOGGING_H

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

void initSDCard();
bool isSDCardAvailable();
void logData(String data);
int readDataFromSD();
void clearLogData();


#endif
