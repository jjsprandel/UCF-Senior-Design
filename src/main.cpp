#include <Arduino.h>
#include "PIRSensor.h"
#include "WiFi.h"
#include <Firebase_ESP_Client.h>
//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Network credentials
#define WIFI_SSID "Boardwalk_Free_WiFi"
#define WIFI_PASSWORD "Pingas01"

// Project API Key
#define API_KEY "AIzaSyBoHGBRg7e_thFtbURSlMAmlvfFNAQ8X1M"

// RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://scan-9ee0b-default-rtdb.firebaseio.com/"

//Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int intValue;
float floatValue;

int count = 0;
bool signupOK = false;

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
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

}

void loop() {
  // // Turn the LED on or off based on PIR state
  // digitalWrite(ledPin, pirState);
  // // Serial.println(pirState);
  // // delay(500);


  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();

    // Example user ID (this should be dynamically assigned)
    String userId = "userId1";  // Replace with the actual user ID
    String userName = "John Doe"; // Replace with the actual user name
    String userUcfId = "123456";  // Replace with the actual UCF ID

    // Write user data to the database path users/userId
    if (Firebase.RTDB.setString(&fbdo, "users/" + userId + "/name", userName)) {
      Serial.println("User name written successfully.");
    } else {
      Serial.println("Failed to write user name: " + fbdo.errorReason());
    }

    if (Firebase.RTDB.setString(&fbdo, "users/" + userId + "/ucfId", userUcfId)) {
      Serial.println("User UCF ID written successfully.");
    } else {
      Serial.println("Failed to write user UCF ID: " + fbdo.errorReason());
    }

    // Set check-in status and timestamp
    bool checkInStatus = true; // Set to true if checking in, false if checking out
    if (Firebase.RTDB.setBool(&fbdo, "users/" + userId + "/checkInStatus", checkInStatus)) {
      Serial.println("Check-in status written successfully.");
    } else {
      Serial.println("Failed to write check-in status: " + fbdo.errorReason());
    }

    // Get the current timestamp
    String currentTimestamp = String(millis()); // Use a proper timestamp format
    if (Firebase.RTDB.setString(&fbdo, "users/" + userId + "/lastCheckIn", currentTimestamp)) {
      Serial.println("Last check-in timestamp written successfully.");
    } else {
      Serial.println("Failed to write last check-in timestamp: " + fbdo.errorReason());
    }

    // Optionally, log the check-in event
    String checkInId = "checkInId1"; // Generate a unique check-in ID for each event
    if (Firebase.RTDB.setString(&fbdo, "checkIns/" + checkInId + "/userId", userId)) {
      Serial.println("Check-in userId logged successfully.");
    } else {
      Serial.println("Failed to log check-in: " + fbdo.errorReason());
    }

    if (Firebase.RTDB.setString(&fbdo, "checkIns/" + checkInId + "/timestamp", currentTimestamp)) {
      Serial.println("Check-in timestamp logged successfully.");
    } else {
      Serial.println("Failed to log check-in: " + fbdo.errorReason());
    }

    // Reading user data back from the database
    if (Firebase.RTDB.getString(&fbdo, "users/" + userId + "/name")) {
      Serial.println("User Name: " + fbdo.stringData());
    } else {
      Serial.println("Failed to read user name: " + fbdo.errorReason());
    }

    if (Firebase.RTDB.getBool(&fbdo, "users/" + userId + "/checkInStatus")) {
      Serial.println("Check-in Status: " + String(fbdo.boolData()));
    } else {
      Serial.println("Failed to read check-in status: " + fbdo.errorReason());
    }
  }

}

void handleMotion() {
  // pirState = digitalRead(pirPin); // Read the state of the PIR sensor



}