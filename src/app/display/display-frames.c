#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "display-frames.h"
#include "gc9a01.h"

// Note: acc to library, draw circle is not optimized
void drawCircularBorder(GC9A01A tft) {
  int cx = tft.width() / 2;
  int cy = tft.height() / 2;
  int radius = min(cx, cy) - 10;

  // Draw a white circle around the display
  tft.draw_circle(cx, cy, radius, GC9A01A_WHITE);
}

// Frame 1: Waiting for NFC tag (Improved aesthetics)
void drawNFCWatingScreen(GC9A01A tft) {
  tft.fill(GC9A01A_BLACK);
  drawCircularBorder(tft);

  // Gradient animation effect for waiting screen
  for (int i = 0; i < 5; i++) {
    tft.draw_cicle(tft.width() / 2, tft.height() / 2, 40 + (i * 10), GC9A01A_DARKGREY);
  }

  tft.setTextColor(GC9A01A_WHITE);
  tft.setTextSize(2);
  tft.setCursor(45, 90);
  tft.println("Tap Your NFC");
  tft.setCursor(30, 130);
  tft.println("Tag to Check-In");
}

// Frame 2: Check-In Success
void drawCheckInSuccessScreen(GC9A01A tft) {
  tft.fill(GC9A01A_BLACK);
  drawCircularBorder(tft);

  tft.setTextColor(GC9A01A_GREEN);
  tft.setTextSize(3);
  tft.setCursor(40, 90);
  tft.println("Check-In");
  tft.setCursor(40, 130);
  tft.println("Success!");

  // Checkmark icon
  // tft.drawLine(110, 160, 130, 180, GC9A01A_GREEN);
  // tft.drawLine(130, 180, 170, 140, GC9A01A_GREEN);
}

// Frame 3: Check-In Failed
void drawCheckInFailedScreen(GC9A01A tft) {
  tft.fill(GC9A01A_BLACK);
  drawCircularBorder(tft);

  tft.setTextColor(GC9A01A_RED);
  tft.setTextSize(3);
  tft.setCursor(40, 90);
  tft.println("Check-In");
  tft.setCursor(40, 130);
  tft.println("Failed!");

  // Cross icon
  // tft.drawLine(120, 160, 160, 200, GC9A01A_RED);
  // tft.drawLine(160, 160, 120, 200, GC9A01A_RED);
}

// Frame 3: Check-In Failed
void drawUserNotFoundScreen(GC9A01A tft) {
  tft.fill(GC9A01A_BLACK);
  drawCircularBorder(tft);

  tft.setTextColor(GC9A01A_RED);
  tft.setTextSize(3);
  tft.setCursor(30, 90);
  tft.println("ERROR! User");
  tft.setCursor(40, 130);
  tft.println("not found");

  // Cross icon
  // tft.drawLine(120, 160, 160, 200, GC9A01A_RED);
  // tft.drawLine(160, 160, 120, 200, GC9A01A_RED);
}

// Frame 4: Check-Out Success
void drawCheckOutSuccessScreen(GC9A01A tft) {
  tft.fill(GC9A01A_BLACK);
  drawCircularBorder(tft);

  tft.setTextColor(GC9A01A_GREEN);
  tft.setTextSize(3);
  tft.setCursor(40, 90);
  tft.println("Check-Out");
  tft.setCursor(40, 130);
  tft.println("Success!");

  // Checkmark icon
  // tft.drawLine(110, 160, 130, 180, GC9A01A_GREEN);
  // tft.drawLine(130, 180, 170, 140, GC9A01A_GREEN);
}

// Frame 5: Check-Out Failed
void drawCheckOutFailedScreen(GC9A01A tft) {
  tft.fill(GC9A01A_BLACK);
  drawCircularBorder(tft);

  tft.setTextColor(GC9A01A_RED);
  tft.setTextSize(3);
  tft.setCursor(40, 90);
  tft.println("Check-Out");
  tft.setCursor(40, 130);
  tft.println("Failed!");

  // Cross icon
  // tft.drawLine(120, 160, 160, 200, GC9A01A_RED);
  // tft.drawLine(160, 160, 120, 200, GC9A01A_RED);
}
