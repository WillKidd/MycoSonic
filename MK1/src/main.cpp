#include <Arduino.h>
#include "signal_processing.h"
#include "data_logging.h"


const int bioSensorPin = A0; // Bioelectric sensor output connected to A0
int filterIndex = 5;         // Index of chosen filter
bool useSDCardData = false; // Whether to use SDCardData as Input
bool logDataToSDCard = true;  // Enable or disable data logging

void setup() {
  Serial.begin(115200); // Start serial communication at 115200 baud

  initSDCard();
  if (!isSDCardPresent()) {
    Serial.println("SD Card not found");
  }
}

void loop() {
    float dataValue;
    int bioValue;

    if (useSDCardData) {
    dataValue = readDataFromSD();
    if (dataValue == -1) {
      Serial.println("End of data or error reading from SD card");
      // Handle end of data or error
    }
  } else {
    bioValue = analogRead(bioSensorPin);// Read the value from the bioelectric sensor
    dataValue = applyFilter(bioValue, filterIndex); // Apply the selected filter
  }

  float voltage = (dataValue / 1023.0) * 3.3; // Convert to voltage
  float milliVolts = voltage * 1000; // Convert voltage to millivolts

  // Format the data for output
  Serial.print(">bio_voltage:");
  Serial.println(milliVolts);

   if (logDataToSDCard && !useSDCardData) {
    logData(""+bioValue);
  }

  delay(10); // Wait for 10 ms
}
