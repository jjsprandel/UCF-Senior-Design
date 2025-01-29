#include "nfc_tag.h"

#define NFC_TAG_TAG "nfc_tag"

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
    if (tag->tagType == NULL)
    {
        // Handle memory allocation failure
        ESP_LOGE(NFC_TAG_TAG, "Failed to allocate memory for tagType");
        return;
    }
    strcpy(tag->tagType, tagType);
    tag->message = (ndefMessage_t *)NULL;
}

bool tagPresent(pn532_t *nfc, uint8_t *uidLength, uint8_t uid[], unsigned long timeout)
{
    bool success;
    *uidLength = 0;

    if (timeout == 0)
        success = pn532_readPassiveTargetID(nfc, PN532_MIFARE_ISO14443A, uid, uidLength, 0);
    else
        success = pn532_readPassiveTargetID(nfc, PN532_MIFARE_ISO14443A, uid, uidLength, timeout);

    return success;
}

void print_tag(nfc_tag_t *tag)
{
    ESP_LOGI(NFC_TAG_TAG, "NFC Tag - %s", tag->tagType);
    char uid_str[2 * tag->uidLength + 1];
    for (int i = 0; i < tag->uidLength; i++)
    {
        sprintf(&uid_str[2 * i], "%02X", tag->uid[i]);
    }
    ESP_LOGI(NFC_TAG_TAG, "UID: %s", uid_str);

    if (tag->message == NULL)
        ESP_LOGI(NFC_TAG_TAG, "Tag has no NDEF message");
    else
        print_message(tag->message);
}