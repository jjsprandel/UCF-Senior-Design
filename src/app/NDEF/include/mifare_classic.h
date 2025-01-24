#ifndef mifare_classic_h
#define mifare_classic_h

#include "nfc_adapter.h"
#include "ndef_message.h"
#include "ndef_record.h"
#include "stdint.h"
#include "pn532.h"

int getBufferSize(int messageLength);
int getNdefStartIndex(uint8_t *data);
bool decodeTlv(uint8_t *data, int &messageLength, int &messageStartIndex);
nfc_tag_t read(pn532_t *PN532, uint8_t *uid, unsigned int uidLength);
bool write(pn532_t *PN532, ndefMessage_t *message, uint8_t *uid, unsigned int uidLength);
bool formatNdef(pn532_t *PN532, uint8_t *uid, unsigned int uidLength);
bool formatMifare(pn532_t *PN532, uint8_t *uid, unsigned int uidLength);

#endif