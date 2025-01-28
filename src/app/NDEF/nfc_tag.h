#ifndef nfc_tag_h
#define nfc_tag_h

#include "ndef_message.h"
#include <stdlib.h>
#include "esp_log.h"
#include "pn532.h"

typedef struct
{
    ndefMessage_t *message;
    uint8_t uid[7];
    uint8_t uidLength;
    char *tagType;
} nfc_tag_t;

void createTag(nfc_tag_t *tag, uint8_t *uid, unsigned int uidLength, char *tagType, uint8_t *data, int messageLength);
void createEmptyTag(nfc_tag_t *tag, uint8_t *uid, unsigned int uidLength, char *tagType);
bool tagPresent(unsigned long timeout);
void print_tag(nfc_tag_t *tag);

#endif