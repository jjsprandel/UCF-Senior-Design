# NFC Reader PlatformIO Project

## Overview
This project integrates NFC reading capabilities with a TFT display using various libraries. The main components include reading NFC tags and displaying information on a TFT screen.

## Dependencies

### Arduino Libraries
- **SPI.h**: SPI communication library.
- **PN532_SPI.h**: PN532 NFC controller library for SPI.
- **PN532.h**: PN532 NFC controller library.
- **NfcAdapter.h**: NFC adapter library.
- **Adafruit_GFX.h**: Adafruit GFX graphics core library.
- **Adafruit_GC9A01A.h**: Adafruit GC9A01A TFT display library.
- **Arduino.h**: Main Arduino library.

### Custom Libraries
- **tft_frames.h**: Custom header for TFT display frames.

### Submodules
This project uses several Git submodules for external libraries. Ensure you initialize and update the submodules after cloning the repository.

```sh
git submodule update --init --recursive