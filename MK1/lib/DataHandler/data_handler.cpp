#include "data_handler.h"


void initSDCard() {
  SD.begin(chipSelect);
  SD.open(filename, FILE_WRITE).close();
}

bool isSDCardAvailable() {
  return SD.exists(filename);
}


void logData(int data) {
  if (!isSDCardAvailable()) {
    Serial.println("SD card communication error - Cannot log data");
    return;
  }
  
  File dataFile = SD.open(filename, FILE_WRITE);
  if (dataFile) {
    dataFile.println(data);
    dataFile.close();
  } else {
    Serial.println("Error opening log file");
  }
}


int readDataFromSD() {
  if (!isSDCardAvailable()) {
    Serial.println("SD card communication error - Cannot read data");
    return -1;
  }
  
  File dataFile = SD.open(filename, FILE_READ);
  if (dataFile) {
    if (dataFile.available()) {
      String dataString = dataFile.readStringUntil('\n');
      dataFile.close();
      int data = dataString.toFloat();
      return data;
    } else {
      dataFile.close();
      return -1;
    }
  } else {
    Serial.println("Error opening datalog.txt for reading");
    return -1;
  }
}

void clearLogData() {
  if (!isSDCardAvailable()) {
    Serial.println("Card failed, or not present - Cannot clear log data");
    return;
  }

  File dataFile = SD.open(filename, FILE_WRITE | O_TRUNC);
  if (dataFile) {
    dataFile.close();
    Serial.println("Log data cleared");
  } else {
    Serial.println("Error clearing log file");
  }
}



/*
Following link for information on the SPI pins.
https://www.arduino.cc/reference/en/language/functions/communication/spi/


They changed their terminology a while back, 
most boards still have the old terminology tho:

old                        <-> new
Master In Slave Out (MISO) <-> Controller In, Peripheral Out (CIPO)
Master Out Slave In (MOSI) <-> Controller Out Peripheral In (COPI)
Slave Select pin (SS)      <-> Chip Select Pin (CS)


Current Config
MISO (Master In Slave Out): Connected to pin 12.
MOSI (Master Out Slave In): Connected to pin 11.
SCK (Serial Clock): Connected to pin 13.
CS (Chip Select): Connected to pin 4. 
*/

