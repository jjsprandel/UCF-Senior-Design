#include "mifare_classic.h"

#define BLOCK_SIZE 16
#define SHORT_TLV_SIZE 2
#define LONG_TLV_SIZE 4
#define NR_SHORTSECTOR (32)         // Number of short sectors on Mifare 1K/4K
#define NR_LONGSECTOR (8)           // Number of long sectors on Mifare 4K
#define NR_BLOCK_OF_SHORTSECTOR (4) // Number of blocks in a short sector
#define NR_BLOCK_OF_LONGSECTOR (16) // Number of blocks in a long sector
// Determine the sector trailer block based on sector number
#define BLOCK_NUMBER_OF_SECTOR_TRAILER(sector) (((sector) < NR_SHORTSECTOR) ? ((sector) * NR_BLOCK_OF_SHORTSECTOR + NR_BLOCK_OF_SHORTSECTOR - 1) : (NR_SHORTSECTOR * NR_BLOCK_OF_SHORTSECTOR + (sector - NR_SHORTSECTOR) * NR_BLOCK_OF_LONGSECTOR + NR_BLOCK_OF_LONGSECTOR - 1))

#define MIFARE_CLASSIC ("Mifare Classic")
#define MIFARE_CLASSIC_TAG ("mifare_classic.c")
#define NDEF_USE_SERIAL
#define MIFARE_CLASSIC_DEBUG

int getBufferSize(int messageLength)
{
    int bufferSize = messageLength;

    // TLV header is 2 or 4 bytes, TLV terminator is 1 byte.
    if (messageLength < 0xFF)
        bufferSize += SHORT_TLV_SIZE + 1;
    else
        bufferSize += LONG_TLV_SIZE + 1;

    if (bufferSize % BLOCK_SIZE != 0)
        bufferSize = ((bufferSize / BLOCK_SIZE) + 1) * BLOCK_SIZE;

    return bufferSize;
}

int getNdefStartIndex(uint8_t *data)
{
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        if (data[i] == 0x0)
        {
            // do nothing, skip
        }
        else if (data[i] == 0x3)
        {
            return i;
        }
        else
        {
#ifdef NDEF_USE_SERIAL
            ESP_LOGI(MIFARE_CLASSIC_TAG, "Unknown TLV 0x%x", data[i]);
#endif
            return -2;
        }
    }

    return -1;
}

// Decode the NDEF data length from the Mifare TLV
// Leading null TLVs (0x0) are skipped
// Assuming T & L of TLV will be in the first block
// messageLength and messageStartIndex written to the parameters
// success or failure status is returned
//
// { 0x3, LENGTH }
// { 0x3, 0xFF, LENGTH, LENGTH }
bool decodeTlv(uint8_t *data, int *messageLength, int *messageStartIndex)
{
    int i = getNdefStartIndex(data);

    if (i < 0 || data[i] != 0x3)
    {
#ifdef NDEF_USE_SERIAL
        ESP_LOGI(MIFARE_CLASSIC_TAG, "Error. Can't decode message length.");
#endif
        return false;
    }
    else
    {
        if (data[i + 1] == 0xFF)
        {
            *messageLength = ((0xFF & data[i + 2]) << 8) | (0xFF & data[i + 3]);
            *messageStartIndex = i + LONG_TLV_SIZE;
        }
        else
        {
            *messageLength = data[i + 1];
            *messageStartIndex = i + SHORT_TLV_SIZE;
        }
    }

    return true;
}

void mifare_read(uint8_t *uid, unsigned int uidLength, nfc_tag_t *tag)
{
    uint8_t key[6] = {0xD3, 0xF7, 0xD3, 0xF7, 0xD3, 0xF7};
    int currentBlock = 4;
    int messageStartIndex = 0;
    int messageLength = 0;
    uint8_t data[BLOCK_SIZE];

    // read first block to get message length
    int success = pn532_mifareclassic_AuthenticateBlock(PN532, uid, uidLength, currentBlock, 0, key);
    if (success)
    {
        success = pn532_mifareclassic_ReadDataBlock(PN532, currentBlock, data);
        if (success)
        {
            if (!decodeTlv(data, &messageLength, &messageStartIndex))
            {
                // createEmptyTag(tag, uid, uidLength, "ERROR");
                return;
                // return NfcTag(uid, uidLength, "ERROR"); // TODO should the error message go in NfcTag?
            }
        }
        else
        {
#ifdef NDEF_USE_SERIAL
            ESP_LOGI(MIFARE_CLASSIC_TAG, "Error. Failed to read block %d", currentBlock);
#endif
            // createEmptyTag(tag, uid, uidLength, MIFARE_CLASSIC);
            return;
            // return NfcTag(uid, uidLength, MIFARE_CLASSIC);
        }
    }
    else
    {
#ifdef NDEF_USE_SERIAL
        ESP_LOGI(MIFARE_CLASSIC_TAG, "Tag is not NDEF formatted.");
#endif
        // TODO set tag.isFormatted = false
        // createEmptyTag(tag, uid, uidLength, MIFARE_CLASSIC);
        return;
        // return NfcTag(uid, uidLength, MIFARE_CLASSIC);
    }

    // this should be nested in the message length loop
    int index = 0;
    int bufferSize = getBufferSize(messageLength);
    uint8_t buffer[bufferSize];

#ifdef MIFARE_CLASSIC_DEBUG
    ESP_LOGI(MIFARE_CLASSIC_TAG, "Message Length %d", messageLength);
    ESP_LOGI(MIFARE_CLASSIC_TAG, "Buffer Size %d", bufferSize);
#endif

    while (index < bufferSize)
    {

        // authenticate on every sector
        if (pn532_mifareclassic_IsFirstBlock(PN532, currentBlock))
        {
            success = pn532_mifareclassic_AuthenticateBlock(PN532, uid, uidLength, currentBlock, 0, key);
            if (!success)
            {
#ifdef NDEF_USE_SERIAL
                ESP_LOGI(MIFARE_CLASSIC_TAG, "Error. Block authentication failed for block %d", currentBlock);
#endif
                // TODO error handling
            }
        }

        // read the data
        success = pn532_mifareclassic_ReadDataBlock(PN532, currentBlock, &buffer[index]);
        if (success)
        {
#ifdef MIFARE_CLASSIC_DEBUG
            ESP_LOGI(MIFARE_CLASSIC_TAG, "Block %d", currentBlock);
            esp_log_buffer_hexdump_internal(MIFARE_CLASSIC_TAG, &buffer[index], BLOCK_SIZE, ESP_LOG_INFO);
            //_nfcShield->PrintHexChar(&buffer[index], BLOCK_SIZE);
#endif
        }
        else
        {
#ifdef NDEF_USE_SERIAL
            ESP_LOGI(MIFARE_CLASSIC_TAG, "Error. Failed to read block %d", currentBlock);
#endif
            // TODO handle errors here
        }

        index += BLOCK_SIZE;
        currentBlock++;

        // skip the trailer block
        if (pn532_mifareclassic_IsTrailerBlock(PN532, currentBlock))
        {
#ifdef MIFARE_CLASSIC_DEBUG
            ESP_LOGI(MIFARE_CLASSIC_TAG, "Skipping block %d", currentBlock);
#endif
            currentBlock++;
        }
    }
    createTag(tag, uid, uidLength, MIFARE_CLASSIC, &buffer[messageStartIndex], bufferSize);
}

bool mifare_write(ndefMessage_t *message, uint8_t *uid, unsigned int uidLength)
{
    uint8_t encoded[message_getEncodedSize(message)];
    message_encode(message, encoded);

    uint8_t buffer[getBufferSize(sizeof(encoded))];
    memset(buffer, 0, sizeof(buffer));

#ifdef MIFARE_CLASSIC_DEBUG
    ESP_LOGI(MIFARE_CLASSIC_TAG, "sizeof(encoded) %d", sizeof(encoded));
    ESP_LOGI(MIFARE_CLASSIC_TAG, "sizeof(buffer) %d", sizeof(buffer));
#endif

    if (sizeof(encoded) < 0xFF)
    {
        buffer[0] = 0x3;
        buffer[1] = sizeof(encoded);
        memcpy(&buffer[2], encoded, sizeof(encoded));
        buffer[2 + sizeof(encoded)] = 0xFE; // terminator
    }
    else
    {
        buffer[0] = 0x3;
        buffer[1] = 0xFF;
        buffer[2] = ((sizeof(encoded) >> 8) & 0xFF);
        buffer[3] = (sizeof(encoded) & 0xFF);
        memcpy(&buffer[4], encoded, sizeof(encoded));
        buffer[4 + sizeof(encoded)] = 0xFE; // terminator
    }

    // Write to tag
    unsigned int index = 0;
    int currentBlock = 4;
    uint8_t key[6] = {0xD3, 0xF7, 0xD3, 0xF7, 0xD3, 0xF7}; // this is Sector 1 - 15 key

    while (index < sizeof(buffer))
    {

        if (pn532_mifareclassic_IsFirstBlock(PN532, currentBlock))
        {
            int success = pn532_mifareclassic_AuthenticateBlock(PN532, uid, uidLength, currentBlock, 0, key);
            if (!success)
            {
#ifdef NDEF_USE_SERIAL
                ESP_LOGI(MIFARE_CLASSIC_TAG, "Error. Block Authentication failed for %d", currentBlock);
#endif
                return false;
            }
        }

        int write_success = pn532_mifareclassic_WriteDataBlock(PN532, currentBlock, &buffer[index]);
        if (write_success)
        {
#ifdef MIFARE_CLASSIC_DEBUG
            ESP_LOGI(MIFARE_CLASSIC_TAG, "Wrote block %d - ", currentBlock);
            esp_log_buffer_hexdump_internal(MIFARE_CLASSIC_TAG, &buffer[index], BLOCK_SIZE, ESP_LOG_INFO);
            // pn532_PrintHexChar(&buffer[index], BLOCK_SIZE); // Replace with ESP function
#endif
        }
        else
        {
#ifdef NDEF_USE_SERIAL
            ESP_LOGI(MIFARE_CLASSIC_TAG, "Write failed %d", currentBlock);
#endif
            return false;
        }
        index += BLOCK_SIZE;
        currentBlock++;

        if (pn532_mifareclassic_IsTrailerBlock(PN532, currentBlock))
        {
// can't write to trailer block
#ifdef MIFARE_CLASSIC_DEBUG
            ESP_LOGI(MIFARE_CLASSIC_TAG, "Skipping block %d", currentBlock);
#endif
            currentBlock++;
        }
    }

    return true;
}

bool mifare_formatNdef(uint8_t *uid, unsigned int uidLength)
{
    uint8_t keya[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    uint8_t emptyNdefMesg[16] = {0x03, 0x03, 0xD0, 0x00, 0x00, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t sectorbuffer0[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t sectorbuffer4[16] = {0xD3, 0xF7, 0xD3, 0xF7, 0xD3, 0xF7, 0x7F, 0x07, 0x88, 0x40, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    bool success = pn532_mifareclassic_AuthenticateBlock(PN532, uid, uidLength, 0, 0, keya);

    if (!success)
    {
#ifdef NDEF_USE_SERIAL
        ESP_LOGI(MIFARE_CLASSIC_TAG, "Unable to authenticate block 0 to enable card formatting!");
#endif
        return false;
    }
    success = pn532_mifareclassic_FormatNDEF(PN532);
    if (!success)
    {
#ifdef NDEF_USE_SERIAL
        ESP_LOGI(MIFARE_CLASSIC_TAG, "Unable to format the card for NDEF");
#endif
    }
    else
    {
        for (int i = 4; i < 64; i += 4)
        {
            success = pn532_mifareclassic_AuthenticateBlock(PN532, uid, uidLength, i, 0, keya);

            if (success)
            {
                if (i == 4) // special handling for block 4
                {
                    if (!(pn532_mifareclassic_WriteDataBlock(PN532, i, emptyNdefMesg)))
                    {
#ifdef NDEF_USE_SERIAL
                        ESP_LOGI(MIFARE_CLASSIC_TAG, "Unable to write block %d", i);
#endif
                    }
                }
                else
                {
                    if (!(pn532_mifareclassic_WriteDataBlock(PN532, i, sectorbuffer0)))
                    {
#ifdef NDEF_USE_SERIAL
                        ESP_LOGI(MIFARE_CLASSIC_TAG, "Unable to write block %d", i);
#endif
                    }
                }
                if (!(pn532_mifareclassic_WriteDataBlock(PN532, i + 1, sectorbuffer0)))
                {
#ifdef NDEF_USE_SERIAL
                    ESP_LOGI(MIFARE_CLASSIC_TAG, "Unable to write block %d", i + 1);
#endif
                }
                if (!(pn532_mifareclassic_WriteDataBlock(PN532, i + 2, sectorbuffer0)))
                {
#ifdef NDEF_USE_SERIAL
                    ESP_LOGI(MIFARE_CLASSIC_TAG, "Unable to write block %d", i + 2);
#endif
                }
                if (!(pn532_mifareclassic_WriteDataBlock(PN532, i + 3, sectorbuffer4)))
                {
#ifdef NDEF_USE_SERIAL
                    ESP_LOGI(MIFARE_CLASSIC_TAG, "Unable to write block %d", i + 3);
#endif
                }
            }
            else
            {
                unsigned int iii = uidLength;
#ifdef NDEF_USE_SERIAL
                ESP_LOGI(MIFARE_CLASSIC_TAG, "Unable to authenticate block %d", i);
#endif
                pn532_readPassiveTargetID(PN532, PN532_MIFARE_ISO14443A, uid, (uint8_t *)&iii, 0);
            }
        }
    }
    return success;
}

bool mifare_formatMifare(uint8_t *uid, unsigned int uidLength)
{
    // The default Mifare Classic key
    uint8_t KEY_DEFAULT_KEYAB[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    uint8_t blockBuffer[16]; // Buffer to store block contents
    uint8_t blankAccessBits[3] = {0xff, 0x07, 0x80};
    uint8_t idx = 0;
    uint8_t numOfSector = 16; // Assume Mifare Classic 1K for now (16 4-block sectors)
    bool success = false;

    for (idx = 0; idx < numOfSector; idx++)
    {
        // Step 1: Authenticate the current sector using key B 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF
        success = pn532_mifareclassic_AuthenticateBlock(PN532, uid, uidLength, BLOCK_NUMBER_OF_SECTOR_TRAILER(idx), 1, (uint8_t *)KEY_DEFAULT_KEYAB);
        if (!success)
        {
#ifdef NDEF_USE_SERIAL
            ESP_LOGI(MIFARE_CLASSIC_TAG, "Authentication failed for sector &d", idx);

#endif
            return false;
        }

        // Step 2: Write to the other blocks
        if (idx == 0)
        {
            memset(blockBuffer, 0, sizeof(blockBuffer));
            if (!(pn532_mifareclassic_WriteDataBlock(PN532, (BLOCK_NUMBER_OF_SECTOR_TRAILER(idx)) - 2, blockBuffer)))
            {
#ifdef NDEF_USE_SERIAL
                ESP_LOGI(MIFARE_CLASSIC_TAG, "Unable to write to sector &d", idx);

#endif
            }
        }
        else
        {
            memset(blockBuffer, 0, sizeof(blockBuffer));
            // this block has not to be overwritten for block 0. It contains Tag id and other unique data.
            if (!(pn532_mifareclassic_WriteDataBlock(PN532, (BLOCK_NUMBER_OF_SECTOR_TRAILER(idx)) - 3, blockBuffer)))
            {
#ifdef NDEF_USE_SERIAL
                ESP_LOGI(MIFARE_CLASSIC_TAG, "Unable to write to sector &d", idx);

#endif
            }
            if (!(pn532_mifareclassic_WriteDataBlock(PN532, (BLOCK_NUMBER_OF_SECTOR_TRAILER(idx)) - 2, blockBuffer)))
            {
#ifdef NDEF_USE_SERIAL
                ESP_LOGI(MIFARE_CLASSIC_TAG, "Unable to write to sector &d", idx);

#endif
            }
        }

        memset(blockBuffer, 0, sizeof(blockBuffer));

        if (!(pn532_mifareclassic_WriteDataBlock(PN532, (BLOCK_NUMBER_OF_SECTOR_TRAILER(idx)) - 1, blockBuffer)))
        {
#ifdef NDEF_USE_SERIAL
            ESP_LOGI(MIFARE_CLASSIC_TAG, "Unable to write to sector &d", idx);

#endif
        }

        // Step 3: Reset both keys to 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF
        memcpy(blockBuffer, KEY_DEFAULT_KEYAB, sizeof(KEY_DEFAULT_KEYAB));
        memcpy(blockBuffer + 6, blankAccessBits, sizeof(blankAccessBits));
        blockBuffer[9] = 0x69;
        memcpy(blockBuffer + 10, KEY_DEFAULT_KEYAB, sizeof(KEY_DEFAULT_KEYAB));

        // Step 4: Write the trailer block
        if (!(pn532_mifareclassic_WriteDataBlock(PN532, (BLOCK_NUMBER_OF_SECTOR_TRAILER(idx)), blockBuffer)))
        {
#ifdef NDEF_USE_SERIAL
            ESP_LOGI(MIFARE_CLASSIC_TAG, "Unable to write trailer block of sector &d", idx);

#endif
        }
    }
    return true;
}