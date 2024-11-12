#include <Arduino.h>
#include "PIRSensor.h"
#include "SCANDatabase.h"

// Network credentials
#define WIFI_SSID "Boardwalk_Free_WiFi"
#define WIFI_PASSWORD "Pingas01"

// API Key and RTDB URL
#define API_KEY "AIzaSyBoHGBRg7e_thFtbURSlMAmlvfFNAQ8X1M"
#define DATABASE_URL "https://scan-9ee0b-default-rtdb.firebaseio.com/"

// Define Firebase Data object
FirebaseData fbdo;

// SCAN database custom drivers
SCANDatabase mySCANDatabase(fbdo, WIFI_SSID, WIFI_PASSWORD, API_KEY, DATABASE_URL);

// Pin defines
const int pirPin = 4;  // Use the GPIO pin where the AM312 OUT pin is connected
const int ledPin = 2;   // Optional: Built-in LED pin on ESP32 to indicate motion

volatile int pirState = 0;

void handleMotion();

void setup() {
  // Make function for PIR sensor setup

  Serial.begin(115200);
  pinMode(pirPin, INPUT);    // Set PIR sensor pin as input
  pinMode(ledPin, OUTPUT);    // Set LED pin as output

  // Attach interrupt to the PIR pin, triggering on either edge
  attachInterrupt(digitalPinToInterrupt(pirPin), handleMotion, CHANGE);

  mySCANDatabase.begin();
}

void loop() {
  // Turn the LED on or off based on PIR state
  digitalWrite(ledPin, pirState);
  // Serial.println(pirState);


  String userUcfId = "6942069420";

  // Check if data is available in the Serial Monitor
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');

    // Check-in if input is "1"
    if (input == "1") {
      mySCANDatabase.checkIn(userUcfId);
      Serial.println("Checked In Command processed");
    }

    // Check-out if input is "2"
    else if (input == "2") {
      mySCANDatabase.checkOut(userUcfId);
      Serial.println("Checked Out command processed");
    }

    else if (input == "3") {
      UserInfo userInfo = mySCANDatabase.getUserInfo(userUcfId);
      Serial.println("Passkey: " + userInfo.passkey);
      Serial.println("Check-in Status: " + String(userInfo.checkInStatus));
    }
  }

  delay(100); // Small delay to avoid flooding the Serial Monitor
}

void handleMotion() {
  pirState = digitalRead(pirPin); // Read the state of the PIR sensor
}