#include <Arduino.h>
#include "PIRSensor.h"

// Pin defines
const int pirPin = 4;  // Use the GPIO pin where the AM312 OUT pin is connected
const int ledPin = 2;   // Optional: Built-in LED pin on ESP32 to indicate motion

volatile int pirState = 0;

void handleMotion();

void setup() {
  Serial.begin(115200);
  pinMode(pirPin, INPUT);    // Set PIR sensor pin as input
  pinMode(ledPin, OUTPUT);    // Set LED pin as output

  // Attach interrupt to the PIR pin, triggering on either edge
  attachInterrupt(digitalPinToInterrupt(pirPin), handleMotion, CHANGE);
}

void loop() {
  // Turn the LED on or off based on PIR state
  digitalWrite(ledPin, pirState);
  // Serial.println(pirState);
  // delay(500);
}

void handleMotion() {
  pirState = digitalRead(pirPin); // Read the state of the PIR sensor
}