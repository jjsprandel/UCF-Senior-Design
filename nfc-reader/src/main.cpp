/**
 * @file main.cpp
 * @brief Main application file for NFC and TFT display integration.
 *
 * This file contains the main logic for reading NFC tags and displaying information
 * on a TFT display using various libraries.
 *
 * @dependencies
 * - SPI.h: SPI communication library
 * - PN532_SPI.h: PN532 NFC controller library for SPI
 * - PN532.h: PN532 NFC controller library
 * - NfcAdapter.h: NFC adapter library
 * - Adafruit_GFX.h: Adafruit GFX graphics core library
 * - Adafruit_GC9A01A.h: Adafruit GC9A01A TFT display library
 * - Arduino.h: Main Arduino library
 * - tft_frames.h: Custom header for TFT display frames
 *
 * @author Cory Brynds
 * @date November 10, 2024
 */

#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532.h>
#include <NfcAdapter.h>
#include "Adafruit_GFX.h"
#include "Adafruit_GC9A01A.h"
#include "Arduino.h"
#include "tft_frames.h"
#include "PIRSensor.h"
#include "SCANDatabase.h"
#include <map>

// Network credentials
#define WIFI_SSID "iPhone (43)"
#define WIFI_PASSWORD "brynds8899"          // Password

// API Key and RTDB URL
#define API_KEY "AIzaSyBoHGBRg7e_thFtbURSlMAmlvfFNAQ8X1M"
#define DATABASE_URL "https://scan-9ee0b-default-rtdb.firebaseio.com/"

#define SPI_CLK 18
#define SPI_MOSI 23
#define SPI_MISO 19
#define SPI_CS_NFC 5

const int pirPin = 4;  // Use the GPIO pin where the AM312 OUT pin is connected
const int ledPin = 2;   // Optional: Built-in LED pin on ESP32 to indicate motion

// Define Firebase Data object
FirebaseData fbdo;

// SCAN database custom drivers
SCANDatabase mySCANDatabase(fbdo, WIFI_SSID, WIFI_PASSWORD, API_KEY, DATABASE_URL);

// Device Instantiations
PN532_SPI pn532spi(SPI, SPI_CS_NFC);
NfcAdapter nfc = NfcAdapter(pn532spi);
Adafruit_GC9A01A tft(SPI_CS_TFT, DC_TFT);

volatile int pirState = 0;

void handleMotion();

// Extract a message string from a text record
String processRecord(NdefRecord record){
  // Serial.print("  TNF: ");
  // Serial.println(record.getTnf());
  // Serial.print("  Type: ");
  // Serial.println(record.getType());

  // The TNF and Type should be used to determine how the application processes payload
  int payloadLength = record.getPayloadLength();
  byte payload[payloadLength];
  record.getPayload(payload);

  // Print the Hex and Printable Characters
  // Serial.print("  Payload (HEX): ");
  // PrintHexChar(payload, payloadLength);

  // Force the data into a String (might work depending on the content)
  // Real code should use smarter processing
  String payloadAsString(reinterpret_cast<char*>(payload+3), payloadLength-3);
  // String payloadAsString = "";
  // for (int c = 0; c < payloadLength; c++)
  //   payloadAsString += (char)payload[c];

  // id is probably blank and will return ""
  // String uid = record.getId();
  // if (uid != "") {
  //   Serial.print("  ID: ");Serial.println(uid);
  // }
  return payloadAsString;
}

// Read an NFC tag in NDEF format and return userID
String readNDEF(bool verbose = 0){
  NfcTag tag = nfc.read();

  if (verbose){
  Serial.println(tag.getTagType());
  Serial.print("UID: ");Serial.println(tag.getUidString());
  }
  
  if (tag.hasNdefMessage()) // every tag won't have a message
  {
    NdefMessage message = tag.getNdefMessage();
    int recordCount = message.getRecordCount();
    if (recordCount == 3){
      if (verbose) Serial.println("Valid ID tag detected!");

      NdefRecord nameRecord = message[0];
      
      String userName = processRecord(message[0]);
      String userID = processRecord(message[1]);
      String userPassword = processRecord(message[2]);

      if (verbose){
      Serial.print("User's name: ");
      Serial.println(userName);

      Serial.print("User ID: ");
      Serial.println(userID);

      // Serial.print("User password: ");
      // Serial.println(userPassword);
      }

      return userID;
    }
    else{
      Serial.print("ERROR! Tag has invalid number of records. Expected 3, received ");
      Serial.println(recordCount);
      return "";
    }
  }
  else
    Serial.println("ERROR! Tag is not in NDEF format.");
    return "";
}

// Format an NFC tag according to NDEF
void formatNDEF(){ 
  bool success = nfc.format();
  if (success)
    Serial.println("\nSuccess, tag formatted as NDEF.");
  else
    Serial.println("\nFormat failed.");
}

// Write ID format to a new NDEF-formatted NFC tag
void writeNDEF(){
  NdefMessage message = NdefMessage();
  message.addTextRecord("Cory Brynds");
  message.addTextRecord("5387641");
  message.addTextRecord("C5Q2CK2d3C3xL3f)8x3)");
  
  bool success = nfc.write(message);
  if (success)
    Serial.println("Success writing ID card.");        
  else
    Serial.println("ID card write failed.");
}

void setup(void) {
  Serial.begin(112500);
  Serial.println("NFC Reader Application");

  pinMode(SPI_CS_TFT, OUTPUT);
  pinMode(SPI_CS_NFC, OUTPUT);
  pinMode(pirPin, INPUT);    // Set PIR sensor pin as input
  pinMode(ledPin, OUTPUT);    // Set LED pin as output

  // Attach interrupt to the PIR pin, triggering on either edge
  attachInterrupt(digitalPinToInterrupt(pirPin), handleMotion, CHANGE);

  mySCANDatabase.begin();

  tft.begin();

  // delay(500);
  nfc.begin();
  // delay(3000);
}

// std::map<String, bool> userStatus;

void loop(void) {
    digitalWrite(ledPin, pirState);

    // If proximity sensor has been triggered. Better method than polling?
    while (pirState) {
        // unsigned long currentTime = millis();

        // bool authSuccess = false;
        if (nfc.tagPresent()) {

            String userID = readNDEF(1);

            if (userID != "") {
                UserInfo userInfo = mySCANDatabase.getUserInfo(userID, 1);

                if (userInfo.validUserInfo){
                  // Serial.println("Passkey: " + userInfo.passkey);

                  if (userInfo.checkInStatus){
                    mySCANDatabase.checkOut(userID);
                    Serial.println("Checking user " + userID + " out");
                    drawCheckOutSuccessScreen(tft);
                  }
                  else{
                    mySCANDatabase.checkIn(userID);
                    Serial.println("Checking user " + userID + " in");
                    drawCheckInSuccessScreen(tft);
                  }
                }
                else{
                  drawUserNotFoundScreen(tft);
                }
        }
        else {
          drawCheckInFailedScreen(tft);
        }
            delay(2000);
            drawNFCWatingScreen(tft);
            nfc.begin(0);  // Reinitialize the NFC reader
    }
}
}

void handleMotion() {
  pirState = digitalRead(pirPin); // Read the state of the PIR sensor
}