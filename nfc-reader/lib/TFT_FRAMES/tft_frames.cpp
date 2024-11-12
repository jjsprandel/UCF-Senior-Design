#include "tft_frames.h"

// Function to draw the circular border (to simulate the actual screen's boundary)
void drawCircularBorder(Adafruit_GC9A01A tft) {
  int cx = tft.width() / 2;
  int cy = tft.height() / 2;
  int radius = min(cx, cy) - 10;  // Avoid edges for readability

  // Draw a circle around the display
  tft.drawCircle(cx, cy, radius, GC9A01A_WHITE);
}


// Frame 1: Waiting for NFC tag
void drawNFCWatingScreen(Adafruit_GC9A01A tft) {
  tft.fillScreen(GC9A01A_BLACK);  // Clear screen with black
  drawCircularBorder(tft);           // Draw the circular boundary

  // Display the "Waiting for NFC tag" message
  tft.setTextColor(GC9A01A_WHITE);
  tft.setTextSize(2);
  tft.setCursor(40, 100);
  tft.println("Waiting for");
  tft.setCursor(40, 130);
  tft.println("NFC tag...");
}

// Frame 2: Authentication Success
void drawAuthSuccessScreen(Adafruit_GC9A01A tft) {
  tft.fillScreen(GC9A01A_BLACK);
  drawCircularBorder(tft);
  
  // Display "Authentication Success" message
  tft.setTextColor(GC9A01A_GREEN);
  tft.setTextSize(2);
  tft.setCursor(40, 100);
  tft.println("Authentication");
  tft.setCursor(40, 130);
  tft.println("Success!");

  // Display a checkmark icon (simple representation)
  tft.drawLine(120, 160, 140, 180, GC9A01A_GREEN);
  tft.drawLine(140, 180, 160, 140, GC9A01A_GREEN);
}

// Frame 3: Authentication Failed
void drawAuthFailedScreen(Adafruit_GC9A01A tft) {
  tft.fillScreen(GC9A01A_BLACK);
  drawCircularBorder(tft);
  
  // Display "Authentication Failed" message
  tft.setTextColor(GC9A01A_RED);
  tft.setTextSize(2);
  tft.setCursor(40, 100);
  tft.println("Authentication");
  tft.setCursor(40, 130);
  tft.println("Failed!");

  // Display a cross mark icon (simple representation)
  tft.drawLine(120, 160, 160, 200, GC9A01A_RED);
  tft.drawLine(160, 160, 120, 200, GC9A01A_RED);
}