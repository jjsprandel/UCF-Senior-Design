#include "nfc_adapter.h"

#define NFC_ADAPTER_TAG "nfc_adapter"

void nfc_adapter_begin(nfc_adapter_t *nfc, boolean verbose)
{
    pn532_spi_init(nfc->PN532, PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);
    pn532_begin(nfc->PN532);

    // shield->begin();

    uint32_t versiondata = pn532_getFirmwareVersion(nfc->PN532);

    if (!versiondata)
    {
#ifdef NDEF_USE_SERIAL
        Serial.print(F("Didn't find PN53x board"));
#endif
        while (1)
            ; // halt
    }

    if (verbose)
    {
#ifdef NDEF_USE_SERIAL
        ESP_LOGI(NFC_ADAPTER_TAG, "Found chip PN5%x", (versiondata >> 24) & 0xFF);
        ESP_LOGI(NFC_ADAPTER_TAG, "Firmware ver. %d.%d", (versiondata >> 16) & 0xFF, (versiondata >> 8) & 0xFF);
#endif
    }
    // configure board to read RFID tags
    pn532_SAMConfig(nfc->PN532);
    ESP_LOGI(NFC_ADAPTER_TAG, "Waiting for an ISO14443A Card ...");
}

void createTag(nfc_tag_t *tag, uint8_t *uid, unsigned int uidLength, char *tagType, uint8_t *data, int messageLength)
{
    tag->uidLength = uidLength;
    tag->uid = uid;
    tag->tagType = (char *)malloc(strlen(tagType) + 1);
    strcpy(tag->tagType, tagType);
    create_ndef_message(tag->message, data, messageLength);
}

void createEmptyTag(nfc_tag_t *tag, uint8_t *uid, unsigned int uidLength, char *tagType)
{
    tag->uidLength = uidLength;
    tag->uid = uid;
    tag->tagType = (char *)malloc(strlen(tagType) + 1);
    strcpy(tag->tagType, tagType);
    tag->message = (ndefMessage_t *)NULL;
}

bool tagPresent(nfc_adapter_t &nfc, unsigned long timeout)
{
    uint8_t success;

    if (timeout == 0)
        success = pn532_readPassiveTargetID(nfc->PN532, PN532_MIFARE_ISO14443A, nfc->uid, &(nfc->uidLength));
    else
        success = pn532_readPassiveTargetID(nfc->PN532, PN532_MIFARE_ISO14443A, nfc->uid, &(nfc->uidLength), timeout);

    return success;
}

bool erase(nfc_adapter_t &nfc)
{
    ndefMessage_t message;
    addEmptyRecord(&message);
    return write(message); // need to implement write
}

bool format(nfc_adapter_t &nfc)
{
    bool success;
    success = mifare_formatNdef(nfc->PN532, nfc->uid, nfc->uidLength);
    return success;
}

bool clean(nfc_adapter_t &nfc)
{
#ifdef NDEF_DEBUG
    ESP_LOGI(NFC_ADAPTER_TAG, "Cleaning MIFARE Classic");
#endif
    bool success = mifare_formatMifare(nfc->PN532, nfc->uid, nfc->uidLength);
    return success;
}

void read(nfc_adapter_t &nfc, nfc_tag_t *tag)
{
#ifdef NDEF_DEBUG
    ESP_LOGI(NFC_ADAPTER_TAG, "Reading MIFARE Classic");
#endif
    nfc_tag_t *tag;
    mifare_read(nfc->PN532, nfc->uid, nfc->uidLength, tag);
}

bool write(nfc_adapter_t &nfc, ndefMessage_t &message)
{
    bool success;
#ifdef NDEF_DEBUG
    ESP_LOGI(NFC_ADAPTER_TAG, "Writing MIFARE Classic");
#endif

    success = mifare_write(nfc->PN532, message, nfc->uid, nfc->uidLength);
    return success;
}