#include <Arduino.h>
#include <Keypad.h>
#include <Wire.h>
#include "pu2clr_pcf8574.h"

// Define the rows and columns for the keypad
const byte ROWS = 4; // Four rows
const byte COLS = 4; // Four columns

PCF pcf;


// Define the keymap
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

// // Define the connections to the Arduino
// byte colPins[COLS] = {13,12,14,27}; // Connect to the row pinouts of the keypad
// byte rowPins[ROWS] = {26,25,33,32}; // Connect to the column pinouts of the keypad

// // Create the Keypad object
// Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  uint8_t i2cAddr = 0x20;
  Serial.begin(9600); // Initialize the serial communication

    // Looking for I2C Adress
    if ((i2cAddr = pcf.lookForDevice()) != 0)
    {
        Serial.print("\nI2C Buss Address: ");
        Serial.print(i2cAddr, HEX);
    }
    else
    {
        Serial.print("\nNo device found");
        Serial.flush();
        while (true);
    }
    
    pcf.setup(i2cAddr); // Starts the device with the found I2C address
}

void loop() {
  // pulling the PCF8574 ports
    for (uint8_t i = 0; i < 7; i++ ) {
      Serial.print(pcf.digitalRead(i));
    }
    Serial.println();
  }