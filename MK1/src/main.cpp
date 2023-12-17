#include <Arduino.h>
#include "signal_processing.h"

const int bioSensorPin = A0; // Bioelectric sensor output connected to A0
int filterIndex = 0;         // Index of chosen filter

void setup() {
  Serial.begin(115200); // Start serial communication at 115200 baud
}

void loop() {
  int bioValue = analogRead(bioSensorPin); // Read the value from the bioelectric sensor
  float filteredValue;
    // Apply the selected filter
  switch (filterIndex) {
    case 0:
      filteredValue = movingAverage(bioValue);
      break;
    case 1:
      filteredValue = lowPassFilter(bioValue);
      break;
    case 2:
      filteredValue = highPassFilter(bioValue);
      break;
    case 3:
      filteredValue = notchFilter(bioValue);
      break;
    case 4:
      filteredValue = medianFilter(bioValue);
      break;
    case 5:
      filteredValue = medianFilter(bioValue);
      break;
    default:
      filteredValue = bioValue;
      break;
  }

  float voltage = (filteredValue / 1023.0) * 3.3; // Convert to voltage
  float milliVolts = voltage * 1000; // Convert voltage to millivolts

  // Format the data for output
  Serial.print(">bio_voltage:");
  Serial.println(milliVolts);

  delay(10); // Wait for 10 ms
}
