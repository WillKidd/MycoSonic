#include <Arduino.h>

const int heartPin = A0; // AD8232 output connected to A0

void setup() {
  Serial.begin(115200); // Start serial communication at 9600 baud
}

void loop() {
  int heartValue = analogRead(heartPin); // Read the value from AD8232
  float voltage = (heartValue / 1023.0) * 3.3; // Convert to voltage
  float milliVolts = voltage * 1000; // Convert voltage to millivolts

  // Format the data for TelePlot
  Serial.print(">voltage:");
  Serial.println(milliVolts);

  delay(10); // Wait for a second
}
