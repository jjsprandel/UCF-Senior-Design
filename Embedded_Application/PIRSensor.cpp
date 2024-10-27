#include "PIRSensor.h"
#include <Arduino.h>

volatile int PIRSensor::motionDetected = 0;
int PIRSensor::pirPin = 0;

// Constructor to initialize the pin and state
PIRSensor::PIRSensor(int pin) {
    pirPin = pin
}

// Method to set up the sensor
void PIRSensor::begin() {
    pinMode(pirPin, INPUT);
    // Attach interrupt to the PIR pin, triggering on either edge
    attachInterrupt(digitalPinToInterrupt(pirPin), handleMotion, CHANGE);
}

void PIRSensor::handleMotion() {
  motionDetected = digitalRead(pirPin);
}
