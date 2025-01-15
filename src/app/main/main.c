#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "sdkconfig.h"
#include "main.h"



static const char *TAG = "example";

/* Use project configuration menu (idf.py menuconfig) to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
//#define BLINK_GPIO CONFIG_BLINK_GPIO
#define BLINK_GPIO 8

static uint8_t s_led_state = 0;





static void blink_led(void)
{
    /* Set the GPIO level according to the state (LOW or HIGH)*/
    gpio_set_level(BLINK_GPIO, s_led_state);
}

static void configure_led(void)
{
    ESP_LOGI(TAG, "Example configured to blink GPIO LED!");
    gpio_reset_pin(BLINK_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
}

void app_main(void)
{
    ESP_LOGI(TAG, "App Main Start");
    
    /* Configure the peripheral according to the LED type */
    configure_led();

    wifi_init();

    vTaskDelay(5000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "OTA update starting");

#if defined(CONFIG_BOOTLOADER_APP_ROLLBACK_ENABLE)
    /**
     * We are treating successful WiFi connection as a checkpoint to cancel rollback
     * process and mark newly updated firmware image as active. For production cases,
     * please tune the checkpoint behavior per end application requirement.
     */
    const esp_partition_t *running = esp_ota_get_running_partition();
    esp_ota_img_states_t ota_state;
    if (esp_ota_get_state_partition(running, &ota_state) == ESP_OK) {
        if (ota_state == ESP_OTA_IMG_PENDING_VERIFY) {
            if (esp_ota_mark_app_valid_cancel_rollback() == ESP_OK) {
                ESP_LOGI(TAG, "App is valid, rollback cancelled successfully");
            } else {
                ESP_LOGE(TAG, "Failed to cancel rollback");
            }
        }
    }
#endif

    // xTaskCreate(&advanced_ota_example_task, "advanced_ota_example_task", 1024 * 8, NULL, 5, NULL);

    while (1) {
        ESP_LOGI(TAG, "Turning the LED %s!", s_led_state == true ? "ON" : "OFF");
        blink_led();
        /* Toggle the LED state */
        s_led_state = !s_led_state;
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

/*

#define PROXIMITY_DETECTED BIT0
#define NFC_READ_SUCCESS BIT1
#define NFC_VALIDATED BIT2

static EventGroupHandle_t event_group;

typedef enum {
    STATE_IDLE,
    STATE_USER_DETECTED,
    STATE_READING_NFC,
    STATE_VALIDATING,
    STATE_DISPLAY_RESULT
} state_t;

static state_t current_state = STATE_IDLE;

// Task to handle proximity sensor
void proximity_task(void *param) {
    while (1) {
        if (proximity_sensor_detected()) {
            xEventGroupSetBits(event_group, PROXIMITY_DETECTED);
        }
        vTaskDelay(pdMS_TO_TICKS(500)); // Check every 500 ms
    }
}

// Task to handle NFC reading
void nfc_task(void *param) {
    while (1) {
        xEventGroupWaitBits(event_group, PROXIMITY_DETECTED, pdTRUE, pdFALSE, portMAX_DELAY);
        if (nfc_read_data()) {
            xEventGroupSetBits(event_group, NFC_READ_SUCCESS);
        }
    }
}

// Task to validate NFC data online
void validation_task(void *param) {
    while (1) {
        xEventGroupWaitBits(event_group, NFC_READ_SUCCESS, pdTRUE, pdFALSE, portMAX_DELAY);
        bool valid = database_validate_nfc();
        if (valid) {
            xEventGroupSetBits(event_group, NFC_VALIDATED);
        }
        current_state = STATE_DISPLAY_RESULT;
    }
}

// Task to update the display based on validation result
void display_task(void *param) {
    while (1) {
        xEventGroupWaitBits(event_group, NFC_VALIDATED, pdTRUE, pdFALSE, portMAX_DELAY);
        display_show_result(true);  // Show success
        vTaskDelay(pdMS_TO_TICKS(5000));  // Display result for 5 seconds
        current_state = STATE_IDLE;
    }
}

// Main State Machine
void app_main(void) {
    // Initialize event group
    event_group = xEventGroupCreate();

    // Initialize hardware and network
    nfc_reader_init();
    display_init();
    wifi_init();
    sensor_init();

    // Create tasks
    xTaskCreate(proximity_task, "Proximity Task", 2048, NULL, 1, NULL);
    xTaskCreate(nfc_task, "NFC Task", 4096, NULL, 1, NULL);
    xTaskCreate(validation_task, "Validation Task", 4096, NULL, 1, NULL);
    xTaskCreate(display_task, "Display Task", 2048, NULL, 1, NULL);

    while (1) {
        switch (current_state) {
            case STATE_IDLE:
                // Wait until proximity is detected
                xEventGroupWaitBits(event_group, PROXIMITY_DETECTED, pdTRUE, pdFALSE, portMAX_DELAY);
                current_state = STATE_USER_DETECTED;
                break;

            case STATE_USER_DETECTED:
                // Wait until NFC data is read
                xEventGroupWaitBits(event_group, NFC_READ_SUCCESS, pdTRUE, pdFALSE, portMAX_DELAY);
                current_state = STATE_READING_NFC;
                break;

            case STATE_READING_NFC:
                // Move to validation state after NFC read
                current_state = STATE_VALIDATING;
                break;

            case STATE_VALIDATING:
                // Wait until NFC validation completes
                xEventGroupWaitBits(event_group, NFC_VALIDATED, pdTRUE, pdFALSE, portMAX_DELAY);
                current_state = STATE_DISPLAY_RESULT;
                break;

            case STATE_DISPLAY_RESULT:
                // Result is displayed; transition back to idle state after showing result
                current_state = STATE_IDLE;
                break;
        }

        vTaskDelay(pdMS_TO_TICKS(100)); // Small delay to prevent rapid state change
    }
}
*/