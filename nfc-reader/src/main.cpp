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

#define TFT_DC 32
#define TFT_CS 33

// Device Instantiations
PN532_SPI pn532spi(SPI, 5);
NfcAdapter nfc = NfcAdapter(pn532spi);
Adafruit_GC9A01A tft(TFT_CS, TFT_DC);

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

// Read an NFC tag in NDEF format
bool readNDEF(){
  NfcTag tag = nfc.read();
  Serial.println(tag.getTagType());
  Serial.print("UID: ");Serial.println(tag.getUidString());

  if (tag.hasNdefMessage()) // every tag won't have a message
  {
    NdefMessage message = tag.getNdefMessage();
    int recordCount = message.getRecordCount();
    if (recordCount == 3){
      Serial.println("Valid ID tag detected!");

      NdefRecord nameRecord = message[0];
      
      String userName = processRecord(message[0]);
      String userID = processRecord(message[1]);
      String userPassword = processRecord(message[2]);

      Serial.print("User's name: ");
      Serial.println(userName);

      Serial.print("User ID: ");
      Serial.println(userID);

      Serial.print("User password: ");
      Serial.println(userPassword);

      return true;
    }
    else{
      Serial.print("ERROR! Tag has invalid number of records. Expected 3, received ");
      Serial.println(recordCount);
      return false;
    }
  }
  else
    Serial.println("ERROR! Tag is not in NDEF format.");
    return false;
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
  message.addTextRecord("5387541");
  message.addTextRecord("C5Q2CK2d3C3xL3f)8x3)");

  bool success = nfc.write(message);
  if (success)
    Serial.println("Success writing ID card.");        
  else
    Serial.println("ID card write failed.");
}

void setup(void) {
  Serial.begin(9600);
  Serial.println("NFC Reader Application");
  nfc.begin();
  tft.begin();
  // drawNFCWatingScreen(tft);
}

void loop(void) {
  bool nfcReaderActive = true;
  bool screenDrawn = false;

  // If proximity sensor has been triggered. Better method than polling?
  if (nfcReaderActive){
    bool authSuccess = false;
    if (nfc.tagPresent())
    {
      authSuccess = readNDEF();
      // writeNDEF();
      // formatNDEF();
      if (authSuccess){
        // drawAuthSuccessScreen(tft);
      }
      else{
        // drawAuthFailedScreen(tft);
      }
      screenDrawn = false;
      delay(3000);
    }
    // else{
    //   if (!screenDrawn){
    //     drawNFCWatingScreen(tft);
    //     screenDrawn = true;
    //   }
    // }
  }
}
