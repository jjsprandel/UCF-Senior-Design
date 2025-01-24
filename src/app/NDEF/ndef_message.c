#include "ndef.h"

int message_getEncodedSize(ndefMessage_t *message)
{
    int size = 0;
    for (int i = 0; i < message->recordCount; i++)
    {
        size += getEncodedSize(message->records[i]);
    }
    return size;
}

void message_encode(ndefMessage_t *message, uint8_t *data)
{
    uint8_t *data_ptr = &data[0];

    for (int i = 0; i < message->recordCount; i++)
    {
        record_encode(message->records[i], data_ptr, i == 0, (i + 1) == message->recordCount);
        data_ptr += record_getEncodedSize(message->records[i]);
    }
}
void create_ndef_message(ndefMessage_t *message, const uint8_t data, const int numBytes)
{
    message->recordCount = 0;

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
    *message->record = record;
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