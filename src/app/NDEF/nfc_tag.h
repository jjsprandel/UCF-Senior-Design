#ifndef nfc_tag_h
#define nfc_tag_h

#include "ndef_message.h"
#include <stdlib.h>

typedef struct
{
    ndefMessage_t *message;
    uint8_t uid[7];
    uint8_t uidLength;
    char *tagType;
} nfc_tag_t;

#endif