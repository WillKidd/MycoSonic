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

