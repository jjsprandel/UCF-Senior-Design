#ifndef NTAG_READER_H
#define NTAG_READER_H

#include "pn532.h"
#include <esp_log.h>
#include <esp_log_internal.h>

void nfc_init();
void findNdefMessage(int *messageLength, int *ndefStartIndex);
int calculateBufferSize(int *messageLength, int *ndefStartIndex);
void ntag2xx_memory_dump_task(void *pvParameters);
void ntag2xx_read_user_id_task(void *pvParameters);
void ntag_erase_task(void *pvParameters);
uint8_t pn532_ntag2xx_WriteNDEF_TEXT(pn532_t *obj, char *text, uint8_t dataLen);
void ntag_write_text_task(void *pvParameters);
void ntag_write_uri_task(void *pvParameters);
#endif