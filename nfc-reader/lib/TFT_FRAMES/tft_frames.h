#pragma once

#include "Adafruit_GC9A01A.h"

#define SPI_CS_TFT 33
#define DC_TFT 32
#define RST_TFT 35

void drawCircularBorder(Adafruit_GC9A01A tft);
void drawNFCWatingScreen(Adafruit_GC9A01A tft);
void drawCheckInSuccessScreen(Adafruit_GC9A01A tft);
void drawCheckInFailedScreen(Adafruit_GC9A01A tft);
void drawUserNotFoundScreen(Adafruit_GC9A01A tft);
void drawCheckOutSuccessScreen(Adafruit_GC9A01A tft);
void drawCheckOutFailedScreen(Adafruit_GC9A01A tft);