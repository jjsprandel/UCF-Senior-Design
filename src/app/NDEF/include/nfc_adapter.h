#ifndef nfc_adapter_h
#define nfc_adapter_h

#include "pn532.h"
#include "mifare_classic.h"
#include "stdint.h"

typedef struct
{
    pn532_t *PN532;
    uint8_t uid[7];
    uint8_t uidLength;
} nfc_adapter_t;

typedef struct
{
    ndefMessage_t *message;
    uint8_t uid;
    uint8_t uidLength;
    char *tagType;
} nfc_tag_t;

void nfc_adapter_begin(nfc_adapter_t *nfc, boolean verbose);
void createTag(nfc_tag_t *tag, uint8_t *uid, unsigned int uidLength, char *tagType, uint8_t *data, int messageLength);
void createEmptyTag(nfc_tag_t *tag, uint8_t *uid, unsigned int uidLength, char *tagType);
bool tagPresent(nfc_adapter_t &nfc, unsigned long timeout);
bool erase(nfc_adapter_t &nfc);
bool format(nfc_adapter_t &nfc);
bool clean(nfc_adapter_t &nfc);
void read(nfc_adapter_t &nfc, nfc_tag_t *tag);
bool write(nfc_adapter_t &nfc, ndefMessage_t &message);

#endif