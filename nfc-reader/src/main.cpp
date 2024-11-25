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

#if 1
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
#else

#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_GC9A01A.h"

// Define pins for display interface. You'll probably need to edit this for
// your own needs:

#if defined(ARDUINO_SEEED_XIAO_RP2040)

// Pinout when using Seed Round Display for XIAO in combination with
// Seeed XIAO RP2040. Other (non-RP2040) XIAO boards, any Adafruit Qt Py
// boards, and other GC9A01A display breakouts will require different pins.
#define TFT_CS D1 // Chip select
#define TFT_DC D3 // Data/command
#define TFT_BL D6 // Backlight control

#else // ALL OTHER BOARDS - EDIT AS NEEDED

// Other RP2040-based boards might not have "D" pin defines as shown above
// and will use GPIO bit numbers. On non-RP2040 boards, you can usually use
// pin numbers silkscreened on the board.
#define TFT_DC  32
#define TFT_CS 33
// If display breakout has a backlight control pin, that can be defined here
// as TFT_BL. On some breakouts it's not needed, backlight is always on.

#endif

// Display constructor for primary hardware SPI connection -- the specific
// pins used for writing to the display are unique to each board and are not
// negotiable. "Soft" SPI (using any pins) is an option but performance is
// reduced; it's rarely used, see header file for syntax if needed.
Adafruit_GC9A01A tft(TFT_CS, TFT_DC);
unsigned long testFillScreen() {
  unsigned long start = micros();
  tft.fillScreen(GC9A01A_BLACK);
  yield();
  tft.fillScreen(GC9A01A_RED);
  yield();
  tft.fillScreen(GC9A01A_GREEN);
  yield();
  tft.fillScreen(GC9A01A_BLUE);
  yield();
  tft.fillScreen(GC9A01A_BLACK);
  yield();
  return micros() - start;
}

unsigned long testText() {
  tft.fillScreen(GC9A01A_BLACK);
  unsigned long start = micros();
  tft.setCursor(0, 0);
  tft.setTextColor(GC9A01A_WHITE);  tft.setTextSize(1);
  tft.println("Hello World!");
  tft.setTextColor(GC9A01A_YELLOW); tft.setTextSize(2);
  tft.println(1234.56);
  tft.setTextColor(GC9A01A_RED);    tft.setTextSize(3);
  tft.println(0xDEADBEEF, HEX);
  tft.println();
  tft.setTextColor(GC9A01A_GREEN);
  tft.setTextSize(5);
  tft.println("Groop");
  tft.setTextSize(2);
  tft.println("I implore thee,");
  tft.setTextSize(1);
  tft.println("my foonting turlingdromes.");
  tft.println("And hooptiously drangle me");
  tft.println("with crinkly bindlewurdles,");
  tft.println("Or I will rend thee");
  tft.println("in the gobberwarts");
  tft.println("with my blurglecruncheon,");
  tft.println("see if I don't!");
  return micros() - start;
}

unsigned long testLines(uint16_t color) {
  unsigned long start, t;
  int           x1, y1, x2, y2,
                w = tft.width(),
                h = tft.height();

  tft.fillScreen(GC9A01A_BLACK);
  yield();

  x1 = y1 = 0;
  y2    = h - 1;
  start = micros();
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = w - 1;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);
  t     = micros() - start; // fillScreen doesn't count against timing

  yield();
  tft.fillScreen(GC9A01A_BLACK);
  yield();

  x1    = w - 1;
  y1    = 0;
  y2    = h - 1;
  start = micros();
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = 0;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);
  t    += micros() - start;

  yield();
  tft.fillScreen(GC9A01A_BLACK);
  yield();

  x1    = 0;
  y1    = h - 1;
  y2    = 0;
  start = micros();
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = w - 1;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);
  t    += micros() - start;

  yield();
  tft.fillScreen(GC9A01A_BLACK);
  yield();

  x1    = w - 1;
  y1    = h - 1;
  y2    = 0;
  start = micros();
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = 0;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);

  yield();
  return micros() - start;
}

unsigned long testFastLines(uint16_t color1, uint16_t color2) {
  unsigned long start;
  int           x, y, w = tft.width(), h = tft.height();

  tft.fillScreen(GC9A01A_BLACK);
  start = micros();
  for(y=0; y<h; y+=5) tft.drawFastHLine(0, y, w, color1);
  for(x=0; x<w; x+=5) tft.drawFastVLine(x, 0, h, color2);

  return micros() - start;
}

unsigned long testRects(uint16_t color) {
  unsigned long start;
  int           n, i, i2,
                cx = tft.width()  / 2,
                cy = tft.height() / 2;

  tft.fillScreen(GC9A01A_BLACK);
  n     = min(tft.width(), tft.height());
  start = micros();
  for(i=2; i<n; i+=6) {
    i2 = i / 2;
    tft.drawRect(cx-i2, cy-i2, i, i, color);
  }

  return micros() - start;
}

unsigned long testFilledRects(uint16_t color1, uint16_t color2) {
  unsigned long start, t = 0;
  int           n, i, i2,
                cx = tft.width()  / 2 - 1,
                cy = tft.height() / 2 - 1;

  tft.fillScreen(GC9A01A_BLACK);
  n = min(tft.width(), tft.height());
  for(i=n; i>0; i-=6) {
    i2    = i / 2;
    start = micros();
    tft.fillRect(cx-i2, cy-i2, i, i, color1);
    t    += micros() - start;
    // Outlines are not included in timing results
    tft.drawRect(cx-i2, cy-i2, i, i, color2);
    yield();
  }

  return t;
}

unsigned long testFilledCircles(uint8_t radius, uint16_t color) {
  unsigned long start;
  int x, y, w = tft.width(), h = tft.height(), r2 = radius * 2;

  tft.fillScreen(GC9A01A_BLACK);
  start = micros();
  for(x=radius; x<w; x+=r2) {
    for(y=radius; y<h; y+=r2) {
      tft.fillCircle(x, y, radius, color);
    }
  }

  return micros() - start;
}

unsigned long testCircles(uint8_t radius, uint16_t color) {
  unsigned long start;
  int           x, y, r2 = radius * 2,
                w = tft.width()  + radius,
                h = tft.height() + radius;

  // Screen is not cleared for this one -- this is
  // intentional and does not affect the reported time.
  start = micros();
  for(x=0; x<w; x+=r2) {
    for(y=0; y<h; y+=r2) {
      tft.drawCircle(x, y, radius, color);
    }
  }

  return micros() - start;
}

unsigned long testTriangles() {
  unsigned long start;
  int           n, i, cx = tft.width()  / 2 - 1,
                      cy = tft.height() / 2 - 1;

  tft.fillScreen(GC9A01A_BLACK);
  n     = min(cx, cy);
  start = micros();
  for(i=0; i<n; i+=5) {
    tft.drawTriangle(
      cx    , cy - i, // peak
      cx - i, cy + i, // bottom left
      cx + i, cy + i, // bottom right
      tft.color565(i, i, i));
  }

  return micros() - start;
}

unsigned long testFilledTriangles() {
  unsigned long start, t = 0;
  int           i, cx = tft.width()  / 2 - 1,
                   cy = tft.height() / 2 - 1;

  tft.fillScreen(GC9A01A_BLACK);
  start = micros();
  for(i=min(cx,cy); i>10; i-=5) {
    start = micros();
    tft.fillTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
      tft.color565(0, i*10, i*10));
    t += micros() - start;
    tft.drawTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
      tft.color565(i*10, i*10, 0));
    yield();
  }

  return t;
}

unsigned long testRoundRects() {
  unsigned long start;
  int           w, i, i2,
                cx = tft.width()  / 2 - 1,
                cy = tft.height() / 2 - 1;

  tft.fillScreen(GC9A01A_BLACK);
  w     = min(tft.width(), tft.height());
  start = micros();
  for(i=0; i<w; i+=6) {
    i2 = i / 2;
    tft.drawRoundRect(cx-i2, cy-i2, i, i, i/8, tft.color565(i, 0, 0));
  }

  return micros() - start;
}

unsigned long testFilledRoundRects() {
  unsigned long start;
  int           i, i2,
                cx = tft.width()  / 2 - 1,
                cy = tft.height() / 2 - 1;

  tft.fillScreen(GC9A01A_BLACK);
  start = micros();
  for(i=min(tft.width(), tft.height()); i>20; i-=6) {
    i2 = i / 2;
    tft.fillRoundRect(cx-i2, cy-i2, i, i, i/8, tft.color565(0, i, 0));
    yield();
  }

  return micros() - start;
}
void setup() {
  Serial.begin(115200);
  Serial.println("GC9A01A Test!");

  tft.begin();

#if defined(TFT_BL)
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH); // Backlight on
#endif // end TFT_BL

  Serial.println(F("Benchmark                Time (microseconds)"));
  delay(10);
  Serial.print(F("Screen fill              "));
  Serial.println(testFillScreen());
  delay(500);

  Serial.print(F("Text                     "));
  Serial.println(testText());
  delay(3000);

  Serial.print(F("Lines                    "));
  Serial.println(testLines(GC9A01A_CYAN));
  delay(500);

  Serial.print(F("Horiz/Vert Lines         "));
  Serial.println(testFastLines(GC9A01A_RED, GC9A01A_BLUE));
  delay(500);

  Serial.print(F("Rectangles (outline)     "));
  Serial.println(testRects(GC9A01A_GREEN));
  delay(500);

  Serial.print(F("Rectangles (filled)      "));
  Serial.println(testFilledRects(GC9A01A_YELLOW, GC9A01A_MAGENTA));
  delay(500);

  Serial.print(F("Circles (filled)         "));
  Serial.println(testFilledCircles(10, GC9A01A_MAGENTA));

  Serial.print(F("Circles (outline)        "));
  Serial.println(testCircles(10, GC9A01A_WHITE));
  delay(500);

  Serial.print(F("Triangles (outline)      "));
  Serial.println(testTriangles());
  delay(500);

  Serial.print(F("Triangles (filled)       "));
  Serial.println(testFilledTriangles());
  delay(500);

  Serial.print(F("Rounded rects (outline)  "));
  Serial.println(testRoundRects());
  delay(500);

  Serial.print(F("Rounded rects (filled)   "));
  Serial.println(testFilledRoundRects());
  delay(500);

  Serial.println(F("Done!"));
}

void loop(void) {
  for(uint8_t rotation=0; rotation<4; rotation++) {
    tft.setRotation(rotation);
    testText();
    delay(1000);
  }
}




#endif