#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <esp_log.h>
#include <esp_log_internal.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "sdkconfig.h"
#include "nfc_init.h"
#include "driver/gpio.h"
#include "pn532.h"

#define BLINK_GPIO 2

#define PN532_SCK 10
#define PN532_MOSI 7
#define PN532_SS 20
#define PN532_MISO 2

static const char *TAG = "APP";

static pn532_t nfc;

void blink_task(void *pvParameter)
{
    esp_rom_gpio_pad_select_gpio(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    while (1)
    {
        gpio_set_level(BLINK_GPIO, 0);
        vTaskDelay(900 / portTICK_PERIOD_MS);
        gpio_set_level(BLINK_GPIO, 1);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void nfc_task(void *pvParameter)
{
    pn532_spi_init(&nfc, PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);
    pn532_begin(&nfc);

    uint32_t versiondata = pn532_getFirmwareVersion(&nfc);
    if (!versiondata)
    {
        ESP_LOGI(TAG, "Didn't find PN53x board");
        while (1)
        {
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }
    // Got ok data, print it out!
    ESP_LOGI(TAG, "Found chip PN5 %lu", (versiondata >> 24) & 0xFF);
    ESP_LOGI(TAG, "Firmware ver. %lu.%lu", (versiondata >> 16) & 0xFF, (versiondata >> 8) & 0xFF);

    // configure board to read RFID tags
    pn532_SAMConfig(&nfc);

    ESP_LOGI(TAG, "Waiting for an ISO14443A Card ...");

    while (1)
    {
        uint8_t success;
        uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0}; // Buffer to store the returned UID
        uint8_t uidLength;                     // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
        uint8_t keyuniversal[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        // uint8_t keyuniversal[6] = {0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5};
        bool authenticated = false;
        uint8_t data[16];

        // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
        // 'uid' will be populated with the UID, and uidLength will indicate
        // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
        success = pn532_readPassiveTargetID(&nfc, PN532_MIFARE_ISO14443A, uid, &uidLength, 0);

        if (success)
        {
            // Display some basic information about the card
            ESP_LOGI(TAG, "Found an ISO14443A card");
            ESP_LOGI(TAG, "UID Length: %d bytes", uidLength);
            ESP_LOGI(TAG, "UID Value:");
            esp_log_buffer_hexdump_internal(TAG, uid, uidLength, ESP_LOG_INFO);

            if (uidLength == 4)
            {
                // We probably have a Mifare Classic card ...
                ESP_LOGI(TAG, "Seems to be a Mifare Classic card (4 byte UID)");
                uint8_t currentblock;

                // Now we try to go through all 16 sectors (each having 4 blocks)
                // authenticating each sector, and then dumping the blocks
                for (currentblock = 0; currentblock < 64; currentblock++)
                {
                    // Check if this is a new block so that we can reauthenticate
                    if (pn532_mifareclassic_IsFirstBlock(&nfc, currentblock))
                        authenticated = false;

                    // If the sector hasn't been authenticated, do so first
                    if (!authenticated)
                    {
                        // Starting of a new sector ... try to to authenticate
                        ESP_LOGI(TAG, "------------------------Sector %d-------------------------", currentblock / 4);

                        if (currentblock == 0)
                        {
                            // This will be 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF for Mifare Classic (non-NDEF!)
                            // or 0xA0 0xA1 0xA2 0xA3 0xA4 0xA5 for NDEF formatted cards using key a,
                            // but keyb should be the same for both (0xFF 0xFF 0xFF 0xFF 0xFF 0xFF)
                            success = pn532_mifareclassic_AuthenticateBlock(&nfc, uid, uidLength, currentblock, 1, keyuniversal);
                        }
                        else
                        {
                            // This will be 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF for Mifare Classic (non-NDEF!)
                            // or 0xD3 0xF7 0xD3 0xF7 0xD3 0xF7 for NDEF formatted cards using key a,
                            // but keyb should be the same for both (0xFF 0xFF 0xFF 0xFF 0xFF 0xFF)
                            success = pn532_mifareclassic_AuthenticateBlock(&nfc, uid, uidLength, currentblock, 1, keyuniversal);
                        }
                        if (success)
                        {
                            authenticated = true;
                        }
                        else
                        {
                            ESP_LOGI(TAG, "Authentication error");
                        }
                    }
                    // If we're still not authenticated just skip the block
                    if (!authenticated)
                    {
                        ESP_LOGI(TAG, "Block %d unable to authenticate", currentblock);
                    }
                    else
                    {
                        // Authenticated ... we should be able to read the block now
                        // Dump the data into the 'data' array
                        success = pn532_mifareclassic_ReadDataBlock(&nfc, currentblock, data);
                        if (success)
                        {
                            // Read successful
                            ESP_LOGI(TAG, "Block %d", currentblock);
                            // Dump the raw data
                            esp_log_buffer_hexdump_internal(TAG, data, 16, ESP_LOG_INFO);
                        }
                        else
                        {
                            // Oops ... something happened
                            ESP_LOGI(TAG, "Block %d unable to read", currentblock);
                        }
                    }
                }
            }
            else
            {
                ESP_LOGI(TAG, "Ooops ... this doesn't seem to be a Mifare Classic card!");
            }
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }

        else
        {
            // PN532 probably timed out waiting for a card
            ESP_LOGI(TAG, "Timed out waiting for a card");
        }
    }
}