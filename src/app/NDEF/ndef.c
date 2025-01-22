#include "ndef.h"

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

typedef struct
{
    ndefRecord_t records[MAX_NDEF_RECORDS];
    uint8_t recordCount;
} ndefMessage_t;

void create_ndef_message(const uint8_t data, const int numBytes)
{
    ndefMessage_t message;
    message.recordCount = 0;

    ndefRecord_t record;
    record.tnf = 0x01;
    record.typeLength = 0x01;
    record.payloadLength = numBytes;
    record.idLength = 0x00;
    record.type = (uint8_t *)malloc(1);
    record.payload = (uint8_t *)malloc(numBytes);
    record.id = (uint8_t *)malloc(1);
    record.type[0] = 0x55;
    memcpy(record.payload, data, numBytes);
    message.records[0] = record;
}

bool addRecord(ndefMessage_t *message, ndefRecord_t *record)
{
    if (message->recordCount >= MAX_NDEF_RECORDS)
    {
        return false;
    }
    message->records[message->recordCount] = record;
    message->recordCount++;
    return true;
}

void addTextRecord(ndefMessage_t *msg, *text)
{
    ndefRecord_t record;
    record.tnf = TNF_WELL_KNOWN;

    uint8_t RTD_TEXT[1] = {0x54};
    record.type = (uint8_t *)malloc(sizeof(RTD_TEXT));
    memcpy(record.type, RTD_TEXT, sizeof(RTD_TEXT));

    int totalLength = strlen("X") + strlen("en") + strlen(text) + 1;
    uint8_t payload[totalLength];
    strcpy(payload, "X");
    strcat(payload, "en");
    strcat(payload, text);
    payload[0] = 2;

    r.payload = (uint8_t *)malloc(totalLength);
    memcpy(r.payload, payload, totalLength);
    r.payloadLength = totalLength;

    record.typeLength = 0x01;
    record.payloadLength = strlen(text);
    record.idLength = 0x00;
    record.id = (uint8_t *)malloc(1);

    addRecord(msg, &record);
}

void addEmptyRecord(ndefMessage_t *msg)
{
    ndefRecord_t record;
    record.tnf = TNF_EMPTY;

    addRecord(msg, &record);
}