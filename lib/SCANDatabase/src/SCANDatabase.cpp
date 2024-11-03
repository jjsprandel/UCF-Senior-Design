#include "SCANDatabase.h"
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

SCANDatabase::SCANDatabase(FirebaseData &fbdo)
    : fbdo(&fbdo), signupOK(false) {}

void SCANLibrary::begin(const String &wifiSSID, const String &wifiPassword, const String &apiKey, const String &databaseUrl) {
    // Initialize WiFi
    WiFi.begin(wifiSSID.c_str(), wifiPassword.c_str());
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(300);
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());

    // Assign the API key and database URL
    config.api_key = apiKey;
    config.database_url = databaseUrl;

    // Sign up
    if (Firebase.signUp(&config, &auth, "", "")) {
        Serial.println("ok");
        signupOK = true;
    } else {
        Serial.printf("%s\n", config.signer.signupError.message.c_str());
    }

    // Assign the token status callback function
    config.token_status_callback = tokenStatusCallback; // See addons/TokenHelper.h

    // Begin Firebase
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);
}


void SCANDatabase::checkIn(const String &userUcfId) {
    // Get and write the current timestamp
    String currentTimestamp = String(millis());
    if (Firebase.RTDB.setString(fbdo, "users/" + userUcfId + "/lastCheckIn", currentTimestamp)) {
        Serial.println("Last check-in timestamp written successfully.");
    } else {
        Serial.println("Failed to write last check-in timestamp: " + fbdo->errorReason());
    }

    // Set check-in status
    if (Firebase.RTDB.setBool(fbdo, "users/" + userUcfId + "/checkInStatus", true)) {
        Serial.println("Check-in status written successfully.");
    } else {
        Serial.println("Failed to write check-in status: " + fbdo->errorReason());
    }

    // Log the check-in event
    lastCheckInId = "checkIn_" + String(millis()); // Use a unique ID for each check-in
    if (Firebase.RTDB.setString(fbdo, "checkIns/" + lastCheckInId + "/userId", userUcfId)) {
        Serial.println("Check-in userUcfId logged successfully.");
    } else {
        Serial.println("Failed to log check-in: " + fbdo->errorReason());
    }

    if (Firebase.RTDB.setString(fbdo, "checkIns/" + lastCheckInId + "/timestamp", currentTimestamp)) {
        Serial.println("Check-in timestamp logged successfully.");
    } else {
        Serial.println("Failed to log check-in: " + fbdo->errorReason());
    }
}

void SCANDatabase::checkOut(const String &userUcfId) {
    if (Firebase.RTDB.setBool(fbdo, "users/" + userUcfId + "/checkInStatus", false)) {
        Serial.println("Check-out status written successfully.");
    } else {
        Serial.println("Failed to write check-out status: " + fbdo->errorReason());
    }
}
