#ifndef ndef_message_h
#define ndef_message_h

#define TNF_EMPTY 0x0
#define TNF_WELL_KNOWN 0x01
#define TNF_MIME_MEDIA 0x02
#define TNF_ABSOLUTE_URI 0x03
#define TNF_EXTERNAL_TYPE 0x04
#define TNF_UNKNOWN 0x05
#define TNF_UNCHANGED 0x06
#define TNF_RESERVED 0x07

typedef struct
{
    ndefRecord_t records[MAX_NDEF_RECORDS];
    uint8_t recordCount;
} ndefMessage_t;

#endif