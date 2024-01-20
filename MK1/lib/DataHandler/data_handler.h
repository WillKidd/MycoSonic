#ifndef DATA_LOGGING_H
#define DATA_LOGGING_H

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>


const int chipSelect = 3; // SD card pin
const String filename = "log.txt";

void initSDCard();
bool isSDCardAvailable();
void logData(int data);
int readDataFromSD();
void clearLogData();


#endif
