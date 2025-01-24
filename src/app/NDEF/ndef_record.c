#include "ndef_record.h"

int record_getEncodedSize(ndefRecord_t *record)
{
    int size = 2; // tnf + typeLength
    if (record->payloadLength > 0xFF)
    {
        size += 4;
    }
    else
    {
        size += 1;
    }

    if (record->idLength)
    {
        size += 1;
    }

    size += (record->typeLength + record->payloadLength + record->idLength);

    return size;
}

void record_encode(ndefRecord_t *record, byte *data, bool firstRecord, bool lastRecord)
{
    uint8_t *data_ptr = &data[0];

    *data_ptr = getTnfByte(record, firstRecord, lastRecord);
    data_ptr += 1;

    *data_ptr = record->typeLength;
    data_ptr += 1;

    if (record->payloadLength <= 0xFF)
    { // short record
        *data_ptr = record->payloadLength;
        data_ptr += 1;
    }
    else
    { // long format
        // 4 bytes but we store length as an int
        data_ptr[0] = 0x0; // (_payloadLength >> 24) & 0xFF;
        data_ptr[1] = 0x0; // (_payloadLength >> 16) & 0xFF;
        data_ptr[2] = (record->payloadLength >> 8) & 0xFF;
        data_ptr[3] = record->payloadLength & 0xFF;
        data_ptr += 4;
    }

    if (_idLength)
    {
        *data_ptr = record->idLength;
        data_ptr += 1;
    }

    // Serial.println(2);
    memcpy(data_ptr, record->type, record->typeLength);
    data_ptr += record->typeLength;

    if (record->idLength)
    {
        memcpy(data_ptr, record->id, record->idLength);
        data_ptr += record->idLength;
    }

    memcpy(data_ptr, record->payload, record->payloadLength);
    data_ptr += record->payloadLength;
}

uint8_t getTnfByte(ndefRecord_t *record, bool firstRecord, bool lastRecord)
{
    int value = record->tnf;

    if (firstRecord)
    { // mb
        value = value | 0x80;
    }

    if (lastRecord)
    { //
        value = value | 0x40;
    }

    // chunked flag is always false for now
    // if (cf) {
    //     value = value | 0x20;
    // }

    if (_payloadLength <= 0xFF)
    {
        value = value | 0x10;
    }

    if (_idLength)
    {
        value = value | 0x8;
    }

    return value;
}