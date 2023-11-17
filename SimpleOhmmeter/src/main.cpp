#include <Arduino.h>

// configuration
#define SENSOR_PIN A0
#define UNITS 1023
#define SAMPLING_RATE_PER_SECOND 1
// in V
#define INPUT_VOLTAGE 5.0 
// in Ohms
#define REFERENCE_RESISTANCE 100

// initialization
int sampling_delay_ms = 1000 / SAMPLING_RATE_PER_SECOND;
int measured_units = 0;
float measured_resistance = 0;

//function declarations
float calculate_output_voltage(float, int);
float calculate_resistance(float, int);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  measured_units = analogRead(SENSOR_PIN);
  measured_resistance = calculate_resistance(INPUT_VOLTAGE, measured_units);
  Serial.println(measured_resistance);
  delay(sampling_delay_ms);
}

//function definitions

float calculate_output_voltage(float input_voltage, int measured_units){
  float output_voltage = (input_voltage * measured_units) / UNITS;
  return output_voltage;
}

float calculate_resistance(float input_voltage, int measured_units){
  float output_voltage = calculate_output_voltage(input_voltage, measured_units);
  float resistance = REFERENCE_RESISTANCE * (1/((input_voltage/output_voltage) - 1));
  return resistance;
}
