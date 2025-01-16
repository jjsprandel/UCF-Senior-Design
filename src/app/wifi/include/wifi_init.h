#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

void wifi_init_sta(void);
void wifi_init_task(void *pvParameter);

extern SemaphoreHandle_t wifi_init_semaphore;