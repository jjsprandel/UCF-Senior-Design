#define USING_SPI 1

#if USING_SPI
#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532.h>
#include <NfcAdapter.h>

PN532_SPI pn532spi(SPI, 5);
NfcAdapter nfc = NfcAdapter(pn532spi);
#else

#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>

PN532_I2C pn532_i2c(Wire);
NfcAdapter nfc = NfcAdapter(pn532_i2c);
#endif

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

void readNDEF(){
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

      /* for (int i = 0; i < recordCount; i++)
      {
        Serial.print("\nNDEF Record ");Serial.println(i+1);
        NdefRecord record = message.getRecord(i);
        // NdefRecord record = message[i]; // alternate syntax

        Serial.print("  TNF: ");Serial.println(record.getTnf());
        Serial.print("  Type: ");Serial.println(record.getType()); // will be "" for TNF_EMPTY

        // The TNF and Type should be used to determine how your application processes the payload
        // There's no generic processing for the payload, it's returned as a byte[]
        int payloadLength = record.getPayloadLength();
        byte payload[payloadLength];
        record.getPayload(payload);

        // Print the Hex and Printable Characters
        Serial.print("  Payload (HEX): ");
        PrintHexChar(payload, payloadLength);

        // Force the data into a String (might work depending on the content)
        // Real code should use smarter processing
        String payloadAsString = "";
        for (int c = 0; c < payloadLength; c++) {
          payloadAsString += (char)payload[c];
        }
        Serial.print("  Payload (as String): ");
        Serial.println(payloadAsString);

        // id is probably blank and will return ""
        String uid = record.getId();
        if (uid != "") {
          Serial.print("  ID: ");Serial.println(uid);
        }
      } */
    }
    else{
      Serial.print("ERROR! Tag has invalid number of records. Expected 3, received ");
      Serial.println(recordCount);
    }
  }
  else
    Serial.println("ERROR! Tag is not in NDEF format.");
}

void formatNDEF(){ 
  bool success = nfc.format();
  if (success)
    Serial.println("\nSuccess, tag formatted as NDEF.");
  else
    Serial.println("\nFormat failed.");
}

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
}

void loop(void) {
  bool nfcReaderActive = true;

  // If proximity sensor has been triggered. Better method than polling?
  if (nfcReaderActive){
    if (nfc.tagPresent())
    {
      // readNDEF();
      // writeNDEF();
      formatNDEF();
    }
  }
}
