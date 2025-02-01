#include "ndef_message.h"

#define NDEF_MESSAGE_TAG "ndef_message"
#define NDEF_DEBUG
#define NDEF_USE_SERIAL

int message_getEncodedSize(ndefMessage_t *message)
{
    int size = 0;
    for (int i = 0; i < message->recordCount; i++)
    {
        size += record_getEncodedSize(&(message->records[i]));
    }
    return size;
}

void message_encode(ndefMessage_t *message, uint8_t *data)
{
    uint8_t *data_ptr = &data[0];

    for (int i = 0; i < message->recordCount; i++)
    {
        record_encode(&(message->records[i]), data_ptr, i == 0, (i + 1) == message->recordCount);
        data_ptr += record_getEncodedSize(&(message->records[i]));
    }
}

void create_ndef_message_empty(ndefMessage_t *message)
{
    message->recordCount = 0;
}

void create_ndef_message(ndefMessage_t *message, const uint8_t *data, const int numBytes)
{
#ifdef NDEF_DEBUG
    ESP_LOGI(NDEF_MESSAGE_TAG, "Decoding %d bytes", numBytes);
    esp_log_buffer_hexdump_internal(NDEF_MESSAGE_TAG, data, numBytes, ESP_LOG_INFO);
    // PrintHexChar(data, numBytes);
// DumpHex(data, numBytes, 16);
#endif

    message->recordCount = 0;
    int index = 0;

    while (index <= numBytes)
    {
        uint8_t tnf_byte = data[index];
        bool me = tnf_byte & 0x40;
        bool sr = tnf_byte & 0x10;
        bool il = tnf_byte & 0x8;
        uint8_t tnf = tnf_byte & 0x7;

        ndefRecord_t record;
        create_ndef_record(&record);

        record.tnf = tnf;
        index++;
        int typeLength = data[index];
        uint32_t payloadLength = 0;

        if (sr)
            payloadLength = data[++index];
        else
        {
            payloadLength =
                ((uint32_t)(data[index]) << 24) | ((uint32_t)(data[index + 1]) << 16) | ((uint32_t)(data[index + 2]) << 8) | (uint32_t)(data[index + 3]);
            // payloadLength =
            //     ((static_cast<uint32_t>)(data[index]) << 24) | (static_cast<uint32_t>(data[index + 1]) << 16) | (static_cast<uint32_t>(data[index + 2]) << 8) | static_cast<uint32_t>(data[index + 3]);
            index += 4;
        }

        int idLength = 0;
        if (il)
            idLength = data[++index];

        index++;
        record.type = (uint8_t *)malloc(typeLength);
        memcpy(record.type, &data[index], typeLength);
        index += typeLength;

        if (il)
        {
            record.id = (uint8_t *)malloc(idLength);
            memcpy(record.id, &data[index], idLength);
            index += idLength;
        }
        record.payload = (uint8_t *)malloc(payloadLength);
        memcpy(record.payload, &data[index], payloadLength);
        index += payloadLength;

        addRecord(message, &record);
        if (me)
            break;
    }
}

bool addRecord(ndefMessage_t *message, ndefRecord_t *record)
{
    if (message->recordCount < MAX_NDEF_RECORDS)
    {
        message->records[message->recordCount] = *record;
        message->recordCount++;
        return true;
    }
    else
    {
#ifdef NDEF_USE_SERIAL
        ESP_LOGI(NDEF_MESSAGE_TAG, "WARNING: Too many records. Increase MAX_NDEF_RECORDS.");
#endif
        return false;
    }
}

void addTextRecord(ndefMessage_t *msg, char *text)
{
    ndefRecord_t record;
    create_ndef_record(&record);

    record.tnf = TNF_WELL_KNOWN;

    uint8_t RTD_TEXT[1] = {0x54};
    setType(&record, RTD_TEXT, sizeof(RTD_TEXT));

    char payloadString[256];
    snprintf(payloadString, sizeof(payloadString), "Xen%s", text);
    uint8_t payload[strlen(payloadString) + 1];
    memcpy(payload, payloadString, strlen(payloadString) + 1);

    payload[0] = strlen("en");

    setPayload(&record, payload, strlen(payloadString));

    addRecord(msg, &record);
}

void addEmptyRecord(ndefMessage_t *msg)
{
    ndefRecord_t record;
    record.tnf = TNF_EMPTY;

    addRecord(msg, &record);
}

void print_message(ndefMessage_t *message)
{
    ESP_LOGI(NDEF_MESSAGE_TAG, "NDEF Message %d%s%d bytes", message->recordCount, message->recordCount == 1 ? ", " : "s, ", message_getEncodedSize(message));

    for (int i = 0; i < message->recordCount; i++)
    {
        print_record(&message->records[i]);
    }
}