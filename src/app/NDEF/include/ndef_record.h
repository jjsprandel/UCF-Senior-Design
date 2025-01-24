#ifndef ndef_record_h
#define ndef_record_h

typdef struct
{
    uint8_t tnf;
    uint8_t typeLength;
    int payloadLength;
    unsigned int idLength;
    uint8_t *type;
    uint8_t *payload;
    uint8_t *id;
} ndefRecord_t;

#endif