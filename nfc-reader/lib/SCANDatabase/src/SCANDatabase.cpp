#include "SCANDatabase.h"
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"
#include <time.h>


void printLocalTime() {
    // Get current time
    time_t now = time(nullptr);
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);

    // Print date and time
    char buffer[32];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
    Serial.println(buffer);
}

String getCurrentDateTimeString() {
    time_t now;
    struct tm timeinfo;
    char buffer[32];
    
    time(&now);
    localtime_r(&now, &timeinfo);
    strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M%S", &timeinfo); // Format: YYYYMMDD_HHMMSS
    return String(buffer);
}

void SCANDatabase::begin() {
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

    // Configure time with NTP server
    configTime(-4 * 3600, 0, "pool.ntp.org");

    // Wait for time to be synchronized
    time_t now = time(nullptr);
    while (now < 1682012400) { // Wait for valid time (before a specific date)
        delay(1000);
        now = time(nullptr);
    }

    // Print the current date and time
    Serial.println("Current Date and Time:");
    printLocalTime();

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

// !! Change this to pass in WiFi_SSID, WiFi_password, API_key, database_url
SCANDatabase::SCANDatabase(FirebaseData &fbdo, const String &wifiSSID, const String &wifiPassword, const String &apiKey, const String &databaseUrl)
    : fbdo(&fbdo), wifiSSID(wifiSSID), wifiPassword(wifiPassword), apiKey(apiKey), databaseUrl(databaseUrl), signupOK(false) {}

// !! Create a getUserInfo(userID) function to get user information from the database
UserInfo SCANDatabase::getUserInfo(const String &userUcfId, bool debugMessages) {
    UserInfo userInfo;

    // Paths for passkey and check-in status
    String passkeyPath = "users/" + userUcfId + "/passkey";
    String checkInStatusPath = "users/" + userUcfId + "/checkInStatus";

    // Retrieve the passkey
    if (Firebase.RTDB.getString(fbdo, passkeyPath)) {
        if (fbdo->dataType() == "string") {
            userInfo.passkey = fbdo->stringData();
            if (debugMessages) Serial.println("Passkey: " + userInfo.passkey);
        }
    } else {
        if (debugMessages) Serial.println("Failed to get passkey: " + fbdo->errorReason());
    }

    // Retrieve the check-in status
    if (Firebase.RTDB.getBool(fbdo, checkInStatusPath)) {
        if (fbdo->dataType() == "boolean") {
            userInfo.checkInStatus = fbdo->boolData();
            if (debugMessages) Serial.println("Check-in Status: " + String(userInfo.checkInStatus));
            userInfo.validUserInfo = 1;
        }
    } else {
        if (debugMessages) Serial.println("Failed to get check-in status: " + fbdo->errorReason());
        userInfo.validUserInfo = 0;
    }

    return userInfo;
}

void SCANDatabase::checkIn(const String &userUcfId, bool debugMessages) {
    // Get and write the current timestamp
    String currentTimestamp = getCurrentDateTimeString(); // Replace with your function for getting the current date and time

    // Create a unique check-in ID
    String checkInId = "checkIn_" + currentTimestamp; // Use date and time for uniqueness

    // Log the event in check-ins
    if (Firebase.RTDB.setString(fbdo, "checkIns/" + checkInId + "/userId", userUcfId)) {
        if (debugMessages) Serial.println("Check-in userUcfId logged successfully.");
    } else {
        if (debugMessages) Serial.println("Failed to log check-in: " + fbdo->errorReason());
    }

    if (Firebase.RTDB.setString(fbdo, "checkIns/" + checkInId + "/timestamp", currentTimestamp)) {
        if (debugMessages) Serial.println("Check-in timestamp: " + currentTimestamp);
    } else {
        if (debugMessages) Serial.println("Failed to log check-in: " + fbdo->errorReason());
    }

    // Update user's last check-in ID and status
    if (Firebase.RTDB.setString(fbdo, "users/" + userUcfId + "/lastCheckInId", checkInId)) {
        if (debugMessages) Serial.println("Last check-in ID written successfully.");
    } else {
        if (debugMessages) Serial.println("Failed to write last check-in ID: " + fbdo->errorReason());
    }

    if (Firebase.RTDB.setBool(fbdo, "users/" + userUcfId + "/checkInStatus", true)) {
        if (debugMessages) Serial.println("Check-in status written successfully.");
    } else {
        if (debugMessages) Serial.println("Failed to write check-in status: " + fbdo->errorReason());
    }
}

void SCANDatabase::checkOut(const String &userUcfId, bool debugMessages) {
    // Get the current timestamp for checkout
    String currentTimestamp = getCurrentDateTimeString(); // Replace with your function for getting the current date and time

    // Create a unique check-out ID
    String checkOutId = "checkOut_" + currentTimestamp; // Use date and time for uniqueness

    // Log the check-out event
    if (Firebase.RTDB.setString(fbdo, "checkOuts/" + checkOutId + "/userId", userUcfId)) {
        if (debugMessages) Serial.println("Check-out userUcfId logged successfully.");
    } else {
        if (debugMessages) Serial.println("Failed to log check-out: " + fbdo->errorReason());
    }

    if (Firebase.RTDB.setString(fbdo, "checkOuts/" + checkOutId + "/timestamp", currentTimestamp)) {
        if (debugMessages) Serial.println("Check-out timestamp logged successfully.");
    } else {
        if (debugMessages) Serial.println("Failed to log check-out: " + fbdo->errorReason());
    }

    // Update user's last check-out ID
    if (Firebase.RTDB.setString(fbdo, "users/" + userUcfId + "/lastCheckOutId", checkOutId)) {
        if (debugMessages) Serial.println("Last check-out ID written successfully.");
    } else {
        if (debugMessages) Serial.println("Failed to write last check-out ID: " + fbdo->errorReason());
    }

    // Update the user's check-in status
    if (Firebase.RTDB.setBool(fbdo, "users/" + userUcfId + "/checkInStatus", false)) {
        if (debugMessages) Serial.println("Check-out status written successfully.");
    } else {
        if (debugMessages) Serial.println("Failed to write check-out status: " + fbdo->errorReason());
    }
}
