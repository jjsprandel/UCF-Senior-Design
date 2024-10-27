#include <Arduino.h>
#include "PIRSensor.h"

// Pin defines
const int pirPin = 4;
const int ledPin = 2;   // Blue on board LED (dev module)

PIRSensor pirSensor(pirPin);

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT); // Set LED pin as output
  pirSensor.begin(); // Initialize the PIR sensor
}

void loop() {
  digitalWrite(ledPin, pirSensor.motionDetected);
}
