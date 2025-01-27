#ifndef nfc_adapter_h
#define nfc_adapter_h

#include "pn532.h"
#include "mifare_classic.h"
#include "ndef_message.h"
#include "stdbool.h"
#include "string.h"
#include <esp_log.h>
#include <esp_log_internal.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    pn532_t *PN532;
    uint8_t uid[7];
    uint8_t uidLength;
} nfc_adapter_t;

extern nfc_adapter_t nfc;

void nfc_adapter_begin(bool verbose);
void createTag(nfc_tag_t *tag, uint8_t *uid, unsigned int uidLength, char *tagType, uint8_t *data, int messageLength);
void createEmptyTag(nfc_tag_t *tag, uint8_t *uid, unsigned int uidLength, char *tagType);
bool tagPresent(unsigned long timeout);
bool nfc_adapter_erase();
bool nfc_adapter_format();
bool nfc_adapter_clean();
void nfc_adapter_read(nfc_tag_t *tag);
bool nfc_adapter_write(ndefMessage_t *message);
void print_tag(nfc_tag_t *tag);

#endif