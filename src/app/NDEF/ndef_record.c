#include "ndef_record.h"

#define NDEF_RECORD_TAG "ndef_record"

void create_ndef_record(ndefRecord_t *record)
{
    record->tnf = 0;
    record->typeLength = 0;
    record->payloadLength = 0;
    record->idLength = 0;
    record->type = (uint8_t *)NULL;
    record->payload = (uint8_t *)NULL;
    record->id = (uint8_t *)NULL;
}

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

void record_encode(ndefRecord_t *record, uint8_t *data, bool firstRecord, bool lastRecord)
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

    if (record->idLength)
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

void setType(ndefRecord_t *record, uint8_t *type, const unsigned int numBytes)
{
    if (record->typeLength)
    {
        free(record->type);
    }

    record->type = (uint8_t *)malloc(numBytes);
    memcpy(record->type, type, numBytes);
    record->typeLength = numBytes;
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

    if (record->payloadLength <= 0xFF)
    {
        value = value | 0x10;
    }

    if (record->idLength)
    {
        value = value | 0x8;
    }

    return value;
}

void print_record(ndefRecord_t *record)
{
    ESP_LOGI(NDEF_RECORD_TAG, "NDEF Record");
    ESP_LOGI(NDEF_RECORD_TAG, "TNF 0x%x ", record->tnf);
    switch (record->tnf)
    {
    case TNF_EMPTY:
        ESP_LOGI(NDEF_RECORD_TAG, "Empty");
        break;
    case TNF_WELL_KNOWN:
        ESP_LOGI(NDEF_RECORD_TAG, "Well Known");
        break;
    case TNF_MIME_MEDIA:
        ESP_LOGI(NDEF_RECORD_TAG, "Mime Media");
        break;
    case TNF_ABSOLUTE_URI:
        ESP_LOGI(NDEF_RECORD_TAG, "Absolute URI");
        break;
    case TNF_EXTERNAL_TYPE:
        ESP_LOGI(NDEF_RECORD_TAG, "External");
        break;
    case TNF_UNKNOWN:
        ESP_LOGI(NDEF_RECORD_TAG, "Unknown");
        break;
    case TNF_UNCHANGED:
        ESP_LOGI(NDEF_RECORD_TAG, "Unchanged");
        break;
    case TNF_RESERVED:
        ESP_LOGI(NDEF_RECORD_TAG, "Reserved");
        break;
    default:
        ESP_LOGI(NDEF_RECORD_TAG, );
    }
    ESP_LOGI(NDEF_RECORD_TAG, "Type Length 0x%x %d", record->typeLength, record->typeLength);
    ESP_LOGI(NDEF_RECORD_TAG, "Payload Length 0x%x %d", record->payloadLength, record->payloadLength);

    if (record->idLength)
    {
        ESP_LOGI(NDEF_RECORD_TAG, "Id Length 0x%x", record->idLength);
    }
    ESP_LOGI(NDEF_RECORD_TAG, "Type ");
    esp_log_buffer_char_internal(NDEF_RECORD_TAG, record->type, record->typeLength, ESP_LOG_INFO);
    // PrintHexChar(_type, _typeLength);
    // TODO chunk large payloads so this is readable
    ESP_LOGI(NDEF_RECORD_TAG, " Payload ");
    esp_log_buffer_char_internal(NDEF_RECORD_TAG, record->payload, record->payloadLength, ESP_LOG_INFO);
    if (record->idLength)
    {
        ESP_LOGI(NDEF_RECORD_TAG, "Id");
        esp_log_buffer_hexdump_internal(NDEF_RECORD_TAG, record->id, record->idLength, ESP_LOG_INFO);
    }
    ESP_LOGI(NDEF_RECORD_TAG, "Record is %d bytes", record_getEncodedSize(record));
}

void setPayload(ndefRecord_t *record, uint8_t *payload, const int numBytes)
{
    if (record->payloadLength)
        free(payload);

    record->payload = (uint8_t *)malloc(numBytes);
    memcpy(record->payload, payload, numBytes);
    record->payloadLength = numBytes;
}