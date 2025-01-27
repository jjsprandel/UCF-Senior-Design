#ifndef mifare_classic_h
#define mifare_classic_h

#include "nfc_tag.h"
#include "nfc_adapter.h"
#include "ndef_message.h"
#include "ndef_record.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "pn532.h"
#include <esp_log.h>
#include <esp_log_internal.h>

int getBufferSize(int messageLength);
int getNdefStartIndex(uint8_t *data);
bool decodeTlv(uint8_t *data, int *messageLength, int *messageStartIndex);
void mifare_read(pn532_t *PN532, uint8_t *uid, unsigned int uidLength, nfc_tag_t *tag);
bool mifare_write(pn532_t *PN532, ndefMessage_t *message, uint8_t *uid, unsigned int uidLength);
bool mifare_formatNdef(pn532_t *PN532, uint8_t *uid, unsigned int uidLength);
bool mifare_formatMifare(pn532_t *PN532, uint8_t *uid, unsigned int uidLength);

#endif