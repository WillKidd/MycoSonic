#include "data_logging.h"

const int chipSelect = 10; // SD card pin

void initSDCard() {
  SD.begin(chipSelect);
}

bool isSDCardPresent() {
  return SD.exists("/");
}

void logData(String data) {
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    return;
  }
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  
  if (dataFile) {
    dataFile.println(data);
    dataFile.close();
  } else {
    Serial.println("Error opening datalog.txt");
  }
}

float readDataFromSD() {
  static File dataFile = SD.open("datalog.txt", FILE_READ);
  
  if (dataFile) {
    if (dataFile.available()) {
      String dataString = dataFile.readStringUntil('\n');
      float data = dataString.toFloat();
      return data;
    } else {
      // Reset the file reading if end of file is reached
      dataFile.seek(0);
      return -1; // or some indicative value
    }
  } else {
    Serial.println("Error opening datalog.txt for reading");
    return -1; // or some indicative value
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
*/

