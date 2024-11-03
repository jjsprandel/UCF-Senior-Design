#include <Arduino.h>
#include "PIRSensor.h"
#include "SCANDatabase.h"

// Network credentials
#define WIFI_SSID "Boardwalk_Free_WiFi"
#define WIFI_PASSWORD "Pingas01"

// Project API Key
#define API_KEY "AIzaSyBoHGBRg7e_thFtbURSlMAmlvfFNAQ8X1M"

// RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://scan-9ee0b-default-rtdb.firebaseio.com/"

// Define Firebase Data object
FirebaseData fbdo;

// Define a global instance of SCANLibrary
SCANDatabase mySCANDatabase(fbdo);

// Pin defines
const int pirPin = 4;  // Use the GPIO pin where the AM312 OUT pin is connected
const int ledPin = 2;   // Optional: Built-in LED pin on ESP32 to indicate motion

volatile int pirState = 0;

void handleMotion();

void setup() {
  // Make function for PIR sensor setup

  // Serial.begin(115200);
  // pinMode(pirPin, INPUT);    // Set PIR sensor pin as input
  // pinMode(ledPin, OUTPUT);    // Set LED pin as output

  // // // // Attach interrupt to the PIR pin, triggering on either edge
  // attachInterrupt(digitalPinToInterrupt(pirPin), handleMotion, CHANGE);

  Serial.begin(115200);
  mySCANDatabase.begin(WIFI_SSID, WIFI_PASSWORD, API_KEY, DATABASE_URL);
}

void loop() {
  // // Turn the LED on or off based on PIR state
  // digitalWrite(ledPin, pirState);
  // // Serial.println(pirState);
  // // delay(500);


  String userUcfId = "6942069420";
  // Test check-in functionality every 30 seconds
    static unsigned long lastCheckInMillis = 0;
    if (millis() - lastCheckInMillis >= 10000) { // 10 seconds
        lastCheckInMillis = millis();
        mySCANDatabase.checkIn(userUcfId); // Call checkIn method
        Serial.println("Checked In");
    }

    // Test check-out functionality every 15 seconds
    static unsigned long lastCheckOutMillis = 0;
    if (millis() - lastCheckOutMillis >= 15000) { // 15 seconds
        lastCheckOutMillis = millis();
        mySCANDatabase.checkOut(userUcfId); // Call checkOut method
        Serial.println("Checked Out");
    }

    // You can add more tests or read data from Firebase if needed
    delay(1000); // Just a delay to avoid flooding the Serial Monitor
}

void handleMotion() {
  // pirState = digitalRead(pirPin); // Read the state of the PIR sensor



}