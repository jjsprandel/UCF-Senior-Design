#include "ntag_reader.h"

extern pn532_t nfc;
#define CARD_READER_TAG "ntag_reader"
#define INIT_TAG "PN532 Initialization"

#define PN532_SCK 10
#define PN532_MOSI 7
#define PN532_SS 20
#define PN532_MISO 2

#define NTAG_213_USER_START 4
#define NTAG_213_USER_STOP 39

#define NTAG_DEBUG_EN

void nfc_init()
{
    pn532_spi_init(&nfc, PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);
    pn532_begin(&nfc);

    uint32_t versiondata = pn532_getFirmwareVersion(&nfc);
    if (!versiondata)
    {
        ESP_LOGI(INIT_TAG, "Didn't find PN53x board");
        while (1)
        {
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }

    ESP_LOGI(INIT_TAG, "Found chip PN5 %lu", (versiondata >> 24) & 0xFF);
    ESP_LOGI(INIT_TAG, "Firmware ver. %lu.%lu", (versiondata >> 16) & 0xFF, (versiondata >> 8) & 0xFF);

    // configure board to read RFID tags
    bool success = pn532_SAMConfig(&nfc);

    if (!success)
    {
        ESP_LOGI(INIT_TAG, "SAMConfig failed");
        while (1)
        {
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }

    ESP_LOGI(INIT_TAG, "Waiting for an ISO14443A Card ...");
}

void findNdefMessage(int *messageLength, int *ndefStartIndex)
{
    uint8_t page;
    uint8_t data[12];
    uint8_t *data_ptr = &data[0];

    bool success = true;
    for (page = 4; page < 6; page++)
    {
        success = success && pn532_ntag2xx_ReadPage(&nfc, page, data_ptr);
        data_ptr += 4;
#ifdef NTAG_DEBUG_EN
        ESP_LOGI(CARD_READER_TAG, "PAGE %c%d: ", (page < 10) ? '0' : ' ', page);
        esp_log_buffer_hexdump_internal(CARD_READER_TAG, data_ptr, 4, ESP_LOG_INFO);
#endif
    }

    if (success)
    {
        if (data[0] == 0x03)
        {
            *messageLength = data[1];
            *ndefStartIndex = 2;
        }
        else if (data[5] == 0x3)
        {
            *messageLength = data[6];
            *ndefStartIndex = 7;
        }
    }
#ifdef NTAG_DEBUG_EN
    ESP_LOGI(CARD_READER_TAG, "Message Length: %d", *messageLength);
    ESP_LOGI(CARD_READER_TAG, "NDEF Start Index: %d", *ndefStartIndex);
#endif
}

int calculateBufferSize(int *messageLength, int *ndefStartIndex)
{
    int bufferSize = *messageLength + *ndefStartIndex + 1;

    if (bufferSize % 4 != 0)
        bufferSize = ((bufferSize / 4) + 1) * 4;

    return bufferSize;
}

void ntag2xx_memory_dump_task(void *pvParameters)
{
    while (1)
    {
        uint8_t success;
        uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0}; // Buffer to store the returned UID
        uint8_t uidLength;                     // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

        // Wait for an NTAG203 card.  When one is found 'uid' will be populated with
        // the UID, and uidLength will indicate the size of the UUID (normally 7)
        success = pn532_readPassiveTargetID(&nfc, PN532_MIFARE_ISO14443A, uid, &uidLength, 0);

        if (success)
        {
            // Display some basic information about the card
            ESP_LOGI(CARD_READER_TAG, "Found an ISO14443A card");
            ESP_LOGI(CARD_READER_TAG, "  UID Length: %d bytes, UID Value:", uidLength);
            esp_log_buffer_hexdump_internal(CARD_READER_TAG, uid, uidLength, ESP_LOG_INFO);

            if (uidLength == 7)
            {
                uint8_t data[32];

                ESP_LOGI(CARD_READER_TAG, "Seems to be an NTAG2xx tag (7 byte UID)");

                for (uint8_t i = NTAG_213_USER_START; i < NTAG_213_USER_STOP; i++)
                {
                    success = pn532_ntag2xx_ReadPage(&nfc, i, data);

                    ESP_LOGI(CARD_READER_TAG, "PAGE %c%d: ", (i < 10) ? '0' : ' ', i);

                    // Display the results, depending on 'success'
                    if (success)
                    {
                        // Dump the page data
                        esp_log_buffer_hexdump_internal(CARD_READER_TAG, data, 4, ESP_LOG_INFO);
                    }
                    else
                    {
                        ESP_LOGI(CARD_READER_TAG, "Unable to read the requested page!");
                    }
                }
            }
            else
            {
                ESP_LOGI(CARD_READER_TAG, "This doesn't seem to be an NTAG203 tag (UUID length != 7 bytes)!");
            }
        }
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

void ntag2xx_read_user_id_task(void *pvParameters)
{
    while (1)
    {
        uint8_t success;
        uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0}; // Buffer to store the returned UID
        uint8_t uidLength;                     // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
        uint8_t dataLength;
        int messageLength, ndefStartIndex, bufferSize;
        // Wait for an NTAG203 card.  When one is found 'uid' will be populated with
        // the UID, and uidLength will indicate the size of the UUID (normally 7)
        success = pn532_readPassiveTargetID(&nfc, PN532_MIFARE_ISO14443A, uid, &uidLength, 0);

        if (success)
        {
            // Display some basic information about the card
            ESP_LOGI(CARD_READER_TAG, "Found an ISO14443A card");
            ESP_LOGI(CARD_READER_TAG, "  UID Length: %d bytes, UID Value:", uidLength);
            esp_log_buffer_hexdump_internal(CARD_READER_TAG, uid, uidLength, ESP_LOG_INFO);

            if (uidLength == 7)
            {
                uint8_t data[32];

                ESP_LOGI(CARD_READER_TAG, "Seems to be an NTAG2xx tag (7 byte UID)");

                // 3.) Check if the NDEF Capability Container (CC) bits are already set
                // in OTP memory (page 3)

                memset(data, 0, 4);
                success = pn532_ntag2xx_ReadPage(&nfc, 3, data);
                if (!success)
                {
                    ESP_LOGI(CARD_READER_TAG, "Unable to read the Capability Container (page 3)");
                    return;
                }
                else
                {
                    // If the tag has already been formatted as NDEF, byte 0 should be:
                    // Byte 0 = Magic Number (0xE1)
                    // Byte 1 = NDEF Version (Should be 0x10)
                    // Byte 2 = Data Area Size (value * 8 bytes)
                    // Byte 3 = Read/Write Access (0x00 for full read and write)
                    if (!((data[0] == 0xE1) && (data[1] == 0x10)))
                    {
                        ESP_LOGI(CARD_READER_TAG, "This doesn't seem to be an NDEF formatted tag.");
                        ESP_LOGI(CARD_READER_TAG, "Page 3 should start with 0xE1 0x10.");
                        return;
                    }
                    else
                    {
                        // 4.) Determine and display the data area size
                        dataLength = data[2] * 8;
                        ESP_LOGI(CARD_READER_TAG, "Tag is NDEF formatted. Data area size = %d bytes", dataLength);
                    }
                } // End of CC check

                findNdefMessage(&messageLength, &ndefStartIndex);
                bufferSize = calculateBufferSize(&messageLength, &ndefStartIndex);
                uint8_t index = 0;
                uint8_t buffer[bufferSize];

                for (uint8_t i = NTAG_213_USER_START; i < NTAG_213_USER_STOP; i++)
                {
                    success = pn532_ntag2xx_ReadPage(&nfc, i, &buffer[index]);

                    if (success)
                    {
#ifdef NTAG_DEBUG_EN
                        // Display the current page number
                        ESP_LOGI(CARD_READER_TAG, "PAGE %c%d: ", (i < 10) ? '0' : ' ', i);
                        esp_log_buffer_hexdump_internal(CARD_READER_TAG, &buffer[index], 4, ESP_LOG_INFO);
#endif
                    }
                    else
                    {
                        ESP_LOGI(CARD_READER_TAG, "Unable to read the requested page!");
                        messageLength = 0;
                        break;
                    }

                    if (index >= messageLength + ndefStartIndex)
                    {
                        break;
                    }
                    index += 4;
                }
                char textPayload[messageLength];
                memcpy(textPayload, &data[ndefStartIndex], messageLength);
                ESP_LOGI(CARD_READER_TAG, "Received NDEF Message: %s", textPayload);
            }
            else
            {
                ESP_LOGI(CARD_READER_TAG, "This doesn't seem to be an NTAG203 tag (UUID length != 7 bytes)!");
            }
        }
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

void ntag_erase_task(void *pvParameters)
{
    while (1)
    {
        uint8_t success;
        uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0}; // Buffer to store the returned UID
        uint8_t uidLength;                     // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

        // Wait for an NTAG203 card.  When one is found 'uid' will be populated with
        // the UID, and uidLength will indicate the size of the UUID (normally 7)
        success = pn532_readPassiveTargetID(&nfc, PN532_MIFARE_ISO14443A, uid, &uidLength, 0);

        if (success)
        {
            // Display some basic information about the card
            ESP_LOGI(CARD_READER_TAG, "Found an ISO14443A card");
            ESP_LOGI(CARD_READER_TAG, "  UID Length: %d bytes, UID Value:", uidLength);
            esp_log_buffer_hexdump_internal(CARD_READER_TAG, uid, uidLength, ESP_LOG_INFO);

            if (uidLength == 7)
            {
                uint8_t data[32];

                // We probably have an NTAG2xx card (though it could be Ultralight as well)
                ESP_LOGI(CARD_READER_TAG, "Seems to be an NTAG2xx tag (7 byte UID)");

                ESP_LOGI(CARD_READER_TAG, "Writing 0x00 0x00 0x00 0x00 to pages 4..39");
                for (uint8_t i = 4; i < NTAG_213_USER_STOP; i++)
                {
                    memset(data, 0, 4);
                    success = pn532_ntag2xx_WritePage(&nfc, i, data);

                    // Display the current page number
                    ESP_LOGI(CARD_READER_TAG, "Page %c%d: %s", (i < 10) ? '0' : ' ', i, success ? "Erased" : "Unable to write to the requested page!");
                }
            }
            else
            {
                ESP_LOGI(CARD_READER_TAG, "This doesn't seem to be an NTAG203 tag (UUID length != 7 bytes)!");
            }
        }
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

void ntag_write_uri_task(void *pvParameters)
{
    while (1)
    {
        uint8_t success;
        uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0}; // Buffer to store the returned UID
        uint8_t uidLength;                     // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
        uint8_t dataLength;
        char *url = "adafruit.com/blog/";
        uint8_t ndefprefix = NDEF_URIPREFIX_HTTP_WWWDOT;

        // Require some user feedback before running this example!
        ESP_LOGI(CARD_READER_TAG, "\r\nPlace your NDEF formatted NTAG2xx tag on the reader to update the NDEF record");

        // 1.) Wait for an NTAG203 card.  When one is found 'uid' will be populated with
        // the UID, and uidLength will indicate the size of the UID (normally 7)
        success = pn532_readPassiveTargetID(&nfc, PN532_MIFARE_ISO14443A, uid, &uidLength, 0);

        // It seems we found a valid ISO14443A Tag!
        if (success)
        {
            // 2.) Display some basic information about the card
            ESP_LOGI(CARD_READER_TAG, "Found an ISO14443A card");
            ESP_LOGI(CARD_READER_TAG, "  UID Length: %d bytes, UID Value:", uidLength);
            esp_log_buffer_hexdump_internal(CARD_READER_TAG, uid, uidLength, ESP_LOG_INFO);

            if (uidLength != 7)
            {
                ESP_LOGI(CARD_READER_TAG, "This doesn't seem to be an NTAG203 tag (UUID length != 7 bytes)!");
            }
            else
            {
                uint8_t data[32];

                ESP_LOGI(CARD_READER_TAG, "Seems to be an NTAG2xx tag (7 byte UID)");

                // 3.) Check if the NDEF Capability Container (CC) bits are already set
                // in OTP memory (page 3)
                memset(data, 0, 4);
                success = pn532_ntag2xx_ReadPage(&nfc, 3, data);
                if (!success)
                {
                    ESP_LOGI(CARD_READER_TAG, "Unable to read the Capability Container (page 3)");
                    return;
                }
                else
                {
                    // If the tag has already been formatted as NDEF, byte 0 should be:
                    // Byte 0 = Magic Number (0xE1)
                    // Byte 1 = NDEF Version (Should be 0x10)
                    // Byte 2 = Data Area Size (value * 8 bytes)
                    // Byte 3 = Read/Write Access (0x00 for full read and write)
                    if (!((data[0] == 0xE1) && (data[1] == 0x10)))
                    {
                        ESP_LOGI(CARD_READER_TAG, "This doesn't seem to be an NDEF formatted tag.");
                        ESP_LOGI(CARD_READER_TAG, "Page 3 should start with 0xE1 0x10.");
                    }
                    else
                    {
                        // 4.) Determine and display the data area size
                        dataLength = data[2] * 8;
                        ESP_LOGI(CARD_READER_TAG, "Tag is NDEF formatted. Data area size = %d bytes", dataLength);

                        // 5.) Erase the old data area
                        ESP_LOGI(CARD_READER_TAG, "Erasing previous data area ");
                        for (uint8_t i = 4; i < (dataLength / 4) + 4; i++)
                        {
                            memset(data, 0, 4);
                            success = pn532_ntag2xx_WritePage(&nfc, i, data);
                            ESP_LOGI(CARD_READER_TAG, ".");
                            if (!success)
                            {
                                ESP_LOGI(CARD_READER_TAG, " ERROR!");
                                return;
                            }
                        }
                        ESP_LOGI(CARD_READER_TAG, " DONE!");

                        // 6.) Try to add a new NDEF URI record
                        ESP_LOGI(CARD_READER_TAG, "Writing URI as NDEF Record ... ");
                        success = pn532_ntag2xx_WriteNDEFURI(&nfc, ndefprefix, url, dataLength);
                        if (success)
                        {
                            ESP_LOGI(CARD_READER_TAG, "DONE!");
                        }
                        else
                        {
                            ESP_LOGI(CARD_READER_TAG, "ERROR! (URI length?)");
                        }

                    } // CC contents NDEF record check
                } // CC page read check
            } // UUID length check

            // Wait a bit before trying again
        } // Start waiting for a new ISO14443A tag
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

void ntag_write_text_task(void *pvParameters)
{
    while (1)
    {
        uint8_t success;
        uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0}; // Buffer to store the returned UID
        uint8_t uidLength;                     // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
        uint8_t dataLength;
        char *userName = "Cory Brynds";

        // Require some user feedback before running this example!
        ESP_LOGI(CARD_READER_TAG, "\r\nPlace your NDEF formatted NTAG2xx tag on the reader to update the NDEF record");

        // 1.) Wait for an NTAG203 card.  When one is found 'uid' will be populated with
        // the UID, and uidLength will indicate the size of the UID (normally 7)
        success = pn532_readPassiveTargetID(&nfc, PN532_MIFARE_ISO14443A, uid, &uidLength, 0);

        // It seems we found a valid ISO14443A Tag!
        if (success)
        {
            // 2.) Display some basic information about the card
            ESP_LOGI(CARD_READER_TAG, "Found an ISO14443A card");
            ESP_LOGI(CARD_READER_TAG, "  UID Length: %d bytes, UID Value:", uidLength);
            esp_log_buffer_hexdump_internal(CARD_READER_TAG, uid, uidLength, ESP_LOG_INFO);

            if (uidLength != 7)
            {
                ESP_LOGI(CARD_READER_TAG, "This doesn't seem to be an NTAG203 tag (UUID length != 7 bytes)!");
            }
            else
            {
                uint8_t data[32];

                ESP_LOGI(CARD_READER_TAG, "Seems to be an NTAG2xx tag (7 byte UID)");

                // 3.) Check if the NDEF Capability Container (CC) bits are already set
                // in OTP memory (page 3)
                memset(data, 0, 4);
                success = pn532_ntag2xx_ReadPage(&nfc, 3, data);
                if (!success)
                {
                    ESP_LOGI(CARD_READER_TAG, "Unable to read the Capability Container (page 3)");
                    return;
                }
                else
                {
                    // If the tag has already been formatted as NDEF, byte 0 should be:
                    // Byte 0 = Magic Number (0xE1)
                    // Byte 1 = NDEF Version (Should be 0x10)
                    // Byte 2 = Data Area Size (value * 8 bytes)
                    // Byte 3 = Read/Write Access (0x00 for full read and write)
                    if (!((data[0] == 0xE1) && (data[1] == 0x10)))
                    {
                        ESP_LOGI(CARD_READER_TAG, "This doesn't seem to be an NDEF formatted tag.");
                        ESP_LOGI(CARD_READER_TAG, "Page 3 should start with 0xE1 0x10.");
                    }
                    else
                    {
                        // 4.) Determine and display the data area size
                        dataLength = data[2] * 8;
                        ESP_LOGI(CARD_READER_TAG, "Tag is NDEF formatted. Data area size = %d bytes", dataLength);

                        // 5.) Erase the old data area
                        ESP_LOGI(CARD_READER_TAG, "Erasing previous data area ");
                        for (uint8_t i = 4; i < (dataLength / 4) + 4; i++)
                        {
                            memset(data, 0, 4);
                            success = pn532_ntag2xx_WritePage(&nfc, i, data);
                            ESP_LOGI(CARD_READER_TAG, ".");
                            if (!success)
                            {
                                ESP_LOGI(CARD_READER_TAG, " ERROR!");
                                return;
                            }
                        }
                        ESP_LOGI(CARD_READER_TAG, " DONE!");

                        // 6.) Try to add a new NDEF URI record
                        ESP_LOGI(CARD_READER_TAG, "Writing Text as NDEF Record ... ");
                        success = pn532_ntag2xx_WriteNDEF_TEXT(&nfc, userName, dataLength);
                        if (success)
                        {
                            ESP_LOGI(CARD_READER_TAG, "DONE!");
                        }
                        else
                        {
                            ESP_LOGI(CARD_READER_TAG, "ERROR! (Text length?)");
                        }

                    } // CC contents NDEF record check
                } // CC page read check
            } // UUID length check

            // Wait a bit before trying again
        } // Start waiting for a new ISO14443A tag
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

uint8_t pn532_ntag2xx_WriteNDEF_TEXT(pn532_t *obj, char *text, uint8_t dataLen)
{
    uint8_t pageBuffer[4] = {0, 0, 0, 0};

    // Remove NDEF record overhead from the URI data (pageHeader below)
    uint8_t wrapperSize = 12;

    // Figure out how long the string is
    uint8_t payloadLen = strlen(text) + strlen("en") + 1;
    // Total field length is: payloadLength + Record Type + Payload Length + Type Length + Record Header

    // Make sure the text payload will fit in dataLen (include 0xFE trailer)
    if ((payloadLen < 1) || (payloadLen + 1 > (dataLen - wrapperSize)))
        return 0;

    // Setup the record header
    // See NFCForum-TS-Type-2-Tag_1.1.pdf for details
    uint8_t pageHeader[12] =
        {
            /* NDEF Lock Control TLV (must be first and always present) */
            0x01, /* Tag Field (0x01 = Lock Control TLV) */
            0x03, /* Payload Length (always 3) */
            0xA0, /* The position inside the tag of the lock bytes (upper 4 = page address, lower 4 = uint8_t offset) */
            0x10, /* Size in bits of the lock area */
            0x44, /* Size in bytes of a page and the number of bytes each lock bit can lock (4 bit + 4 bits) */
            /* NDEF Message TLV - URI Record */
            0x03,           /* Tag Field (0x03 = NDEF Message) */
            payloadLen + 5, /* Payload Length (not including 0xFE trailer) */
            0xD1,           /* NDEF Record Header (TNF=0x1:Well known record + SR + ME + MB) */
            0x01,           /* Type Length for the record type indicator */
            payloadLen + 1, /* Payload len */
            0x54,           /* Record Type Indicator (0x54 or 'T' = Text Record) */
            0x02,           /* Status Byte (UTF-8, encoding length = 1) */
        };

    // Write 12 uint8_t header (three pages of data starting at page 4)
    memcpy(pageBuffer, pageHeader, 4);
    if (!(pn532_ntag2xx_WritePage(obj, 4, pageBuffer)))
        return 0;
    memcpy(pageBuffer, pageHeader + 4, 4);
    if (!(pn532_ntag2xx_WritePage(obj, 5, pageBuffer)))
        return 0;
    memcpy(pageBuffer, pageHeader + 8, 4);
    if (!(pn532_ntag2xx_WritePage(obj, 6, pageBuffer)))
        return 0;

    // Write URI (starting at page 7)
    uint8_t currentPage = 7;
    char textPayload[payloadLen];
    sprintf(textPayload, "en%s", text);
    char *textCpy = textPayload;

    while (payloadLen)
    {
        if (payloadLen < 4)
        {
            memset(pageBuffer, 0, 4);
            memcpy(pageBuffer, textCpy, payloadLen);
            pageBuffer[payloadLen] = 0xFE; // NDEF record footer
            if (!(pn532_ntag2xx_WritePage(obj, currentPage, pageBuffer)))
                return 0;
            // DONE!
            return 1;
        }
        else if (payloadLen == 4)
        {
            memcpy(pageBuffer, textCpy, payloadLen);
            if (!(pn532_ntag2xx_WritePage(obj, currentPage, pageBuffer)))
                return 0;
            memset(pageBuffer, 0, 4);
            pageBuffer[0] = 0xFE; // NDEF record footer
            currentPage++;
            if (!(pn532_ntag2xx_WritePage(obj, currentPage, pageBuffer)))
                return 0;
            // DONE!
            return 1;
        }
        else
        {
            // More than one page of data left
            memcpy(pageBuffer, textCpy, 4);
            if (!(pn532_ntag2xx_WritePage(obj, currentPage, pageBuffer)))
                return 0;
            currentPage++;
            textCpy += 4;
            payloadLen -= 4;
        }
    }

    // Seems that everything was OK (?!)
    return 1;
}