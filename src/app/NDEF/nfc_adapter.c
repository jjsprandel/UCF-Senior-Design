#include "nfc_adapter.h"

#define NFC_ADAPTER_TAG "nfc_adapter"
#define PN532_SCK 10
#define PN532_MOSI 7
#define PN532_SS 20
#define PN532_MISO 2
#define NDEF_DEBUG
#define NDEF_USE_SERIAL

void nfc_adapter_begin(bool verbose)
{
    nfc.PN532 = (pn532_t *)malloc(sizeof(pn532_t));
    pn532_spi_init(nfc.PN532, PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);
    pn532_begin(nfc.PN532);

    // shield->begin();

    uint32_t versiondata = pn532_getFirmwareVersion(nfc.PN532);

    if (!versiondata)
    {
#ifdef NDEF_USE_SERIAL
        ESP_LOGI(NFC_ADAPTER_TAG, "Didn't find PN53x board");
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
    pn532_SAMConfig(nfc.PN532);
    ESP_LOGI(NFC_ADAPTER_TAG, "Waiting for an ISO14443A Card ...");
}

void createTag(nfc_tag_t *tag, uint8_t *uid, unsigned int uidLength, char *tagType, uint8_t *data, int messageLength)
{
    tag->uidLength = uidLength;
    memcpy(tag->uid, uid, uidLength);
    // tag->uid = uid;
    tag->tagType = (char *)malloc(strlen(tagType) + 1);
    strcpy(tag->tagType, tagType);
    create_ndef_message(tag->message, data, messageLength);
}

void createEmptyTag(nfc_tag_t *tag, uint8_t *uid, unsigned int uidLength, char *tagType)
{
    tag->uidLength = uidLength;
    memcpy(tag->uid, uid, uidLength);
    tag->tagType = (char *)malloc(strlen(tagType) + 1);
    strcpy(tag->tagType, tagType);
    tag->message = (ndefMessage_t *)NULL;
}

bool tagPresent(unsigned long timeout)
{
    uint8_t success;

    if (timeout == 0)
        success = pn532_readPassiveTargetID(nfc.PN532, PN532_MIFARE_ISO14443A, nfc.uid, &(nfc.uidLength), 0);
    else
        success = pn532_readPassiveTargetID(nfc.PN532, PN532_MIFARE_ISO14443A, nfc.uid, &(nfc.uidLength), timeout);

    return success;
}

void print_tag(nfc_tag_t *tag)
{
    ESP_LOGI(NFC_ADAPTER_TAG, "NFC Tag - %s", tag->tagType);
    ESP_LOGI(NFC_ADAPTER_TAG, "UID: %s", tag->uid);

    if (tag->message == NULL)
        ESP_LOGI(NFC_ADAPTER_TAG, "Tag has no NDEF message");
    else
        print_message(tag->message);
}

bool nfc_adapter_erase()
{
    ndefMessage_t message;
    addEmptyRecord(&message);
    return nfc_adapter_write(&message); // need to implement write
}

bool nfc_adapter_format()
{
    bool success;
    success = mifare_formatNdef(nfc.PN532, nfc.uid, nfc.uidLength);
    return success;
}

bool nfc_adapter_clean()
{
#ifdef NDEF_DEBUG
    ESP_LOGI(NFC_ADAPTER_TAG, "Cleaning MIFARE Classic");
#endif
    bool success = mifare_formatMifare(nfc.PN532, nfc.uid, nfc.uidLength);
    return success;
}

void nfc_adapter_read(nfc_tag_t *tag)
{
#ifdef NDEF_DEBUG
    ESP_LOGI(NFC_ADAPTER_TAG, "Reading MIFARE Classic");
#endif

    mifare_read(nfc.PN532, nfc.uid, nfc.uidLength, tag);
}

bool nfc_adapter_write(ndefMessage_t *message)
{
    bool success;
#ifdef NDEF_DEBUG
    ESP_LOGI(NFC_ADAPTER_TAG, "Writing MIFARE Classic");
#endif

    success = mifare_write(nfc.PN532, message, nfc.uid, nfc.uidLength);
    return success;
}