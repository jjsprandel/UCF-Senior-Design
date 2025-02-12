
#include "main.h"

// void blink_led_task(void *pvParameter)
// {
//     while (1)
//     {
//         s_led_state = !s_led_state;

//         if (current_kiosk_state == STATE_WIFI_INIT)
//         {
//             vTaskDelay(200 / portTICK_PERIOD_MS);
//         }
//         else
//         {
//             vTaskDelay(1200 / portTICK_PERIOD_MS);
//         }

//         /* If the addressable LED is enabled */
// if (s_led_state)
// {
//     /* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for each color */
//     if (current_kiosk_state == STATE_WIFI_INIT)
//     {
//         led_strip_set_pixel(led_strip, 0, 100, 0, 0);
//     }
//     else
//     {
//         led_strip_set_pixel(led_strip, 0, 0, 0, 50);
//     }
//     /* Refresh the strip to send data */
//     led_strip_refresh(led_strip);
// }
// else
// {
//     /* Set all LED off to clear all pixels */
//     led_strip_clear(led_strip);
// }
// }
// }

// static void configure_led(void)
// {
//     ESP_LOGI(TAG, "Configured to blink addressable LED!");
//     /* LED strip initialization with the GPIO and pixels number*/
//     led_strip_config_t strip_config = {
//         .strip_gpio_num = BLINK_GPIO,
//         .max_leds = 1, // at least one LED on board
//     };

//     led_strip_rmt_config_t rmt_config = {
//         .resolution_hz = 10 * 1000 * 1000, // 10MHz
//         .flags.with_dma = false,
//     };
//     ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));

//     /* Set all LED off to clear all pixels */
//     led_strip_clear(led_strip);
// }

// Task to handle proximity sensor
void proximity_task(void *param)
{
    while (1)
    {
        xEventGroupWaitBits(event_group, IDLE_BIT, pdFALSE, pdFALSE, portMAX_DELAY);
        if (gpio_get_level(PIR_GPIO))
        {
            xEventGroupSetBits(event_group, ENTERING_ID_BIT);
            xEventGroupClearBits(event_group, IDLE_BIT);
#ifdef MAIN_DEBUG
            ESP_LOGI(TAG, "Proximity Detected");
#endif
        }
        vTaskDelay(pdMS_TO_TICKS(500)); // Check every 500 ms
    }
}

// // Task to handle NFC reading
void nfc_scan_id_task(void *param)
{
    while (1)
    {
        xEventGroupWaitBits(event_group, ENTERING_ID_BIT, pdFALSE, pdFALSE, portMAX_DELAY);
        bool readIdSuccess = read_user_id(nfcUserID);

        if (readIdSuccess)
        {
            xEventGroupClearBits(event_group, ENTERING_ID_BIT);
            xEventGroupSetBits(event_group, ID_ENTERED_SUCCESS_BIT);
#ifdef MAIN_DEBUG
            ESP_LOGI(TAG, "NFC Read Success. ID is: %s", nfcUserID);
#endif
            nfcReadFlag = true;
        }
    }
}

void keypad_enter_id_task(void *param)
{
    while (1)
    {
        xEventGroupWaitBits(event_group, ENTERING_ID_BIT, pdFALSE, pdFALSE, portMAX_DELAY);
        if (keypad_task_handle == NULL)
        {
#ifdef MAIN_DEBUG
            ESP_LOGI(TAG, "Starting Keypad Task");
#endif
            xTaskCreate(keypad_handler, "keypad_task", 4096, NULL, 1, &keypad_task_handle);
        }

        xEventGroupClearBits(event_group, ENTERING_ID_BIT);
        xEventGroupWaitBits(event_group, ID_ENTERED_SUCCESS_BIT, pdFALSE, pdFALSE, portMAX_DELAY);

#ifdef MAIN_DEBUG
        if (!nfcReadFlag)
            ESP_LOGI(TAG, "Keypad Read Success. ID is: %s", keypad_buffer.elements);
#endif

        if (keypad_task_handle != NULL)
        {
#ifdef MAIN_DEBUG
            ESP_LOGI(TAG, "Deleting Keypad Task");
#endif
            vTaskDelete(keypad_task_handle);
            keypad_task_handle = NULL;
        }
    }
}

// Task to validate NFC data online
void validation_task(void *param)
{
    while (1)
    {
        xEventGroupWaitBits(event_group, ID_ENTERED_SUCCESS_BIT, pdTRUE, pdFALSE, portMAX_DELAY);

        // bool idIsValid = database_validate_nfc(userID);
        if (idIsValid)
        {
            xEventGroupSetBits(event_group, ID_AUTHENTICATED_BIT);
#ifdef MAIN_DEBUG
            ESP_LOGI(TAG, "ID %s found in database. ID accepted.", !nfcReadFlag ? keypad_buffer.elements : nfcUserID);
#endif
        }
        else
        {
            keypadEnteredFlag = false;
#ifdef MAIN_DEBUG
            ESP_LOGI(TAG, "ID %s not found in database. ID denied.", !nfcReadFlag ? keypad_buffer.elements : nfcUserID);
#endif
        }
        nfcReadFlag = false;
        clear_buffer();
    }
}

// Task to update the display
void display_task(void *param)
{
    while (1)
    {
        if (current_state != prev_state)
        {
            lv_obj_delete(disp_obj);
            switch (current_state)
            {
            case STATE_IDLE:
                _lock_acquire(&lvgl_api_lock);
                disp_obj = display_idle(display);
                _lock_release(&lvgl_api_lock);
                break;
            case STATE_USER_DETECTED:
                _lock_acquire(&lvgl_api_lock);
                disp_obj = display_idle(display);
                _lock_release(&lvgl_api_lock);
                break;
            case STATE_VALIDATING:
                _lock_acquire(&lvgl_api_lock);
                disp_obj = display_transmitting(display);
                _lock_release(&lvgl_api_lock);
                break;
            case STATE_DISPLAY_RESULT:
#ifdef MAIN_DEBUG
                ESP_LOGI(TAG, "Displaying validation results: %s", idIsValid ? "Success" : "Failed");
#endif
                _lock_acquire(&lvgl_api_lock);
                disp_obj = display_check_in_success(display);
                _lock_release(&lvgl_api_lock);
                break;
            default:
                _lock_acquire(&lvgl_api_lock);
                disp_obj = display_idle(display);
                _lock_release(&lvgl_api_lock);
                break;
            }
        }
        prev_state = current_state;
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// Main State Machine
void app_main(void)
{
    ESP_LOGI(TAG, "Starting Kiosk Application");
    // Initialize event group
    event_group = xEventGroupCreate();

    // Initialize hardware and network
    i2c_master_init();
    nfc_init();
    gc9a01_init();
    sensor_init();
    // wifi_init();

    // Create tasks
    xTaskCreate(proximity_task, "Proximity Task", 2048, NULL, 1, NULL);
    xTaskCreate(nfc_scan_id_task, "nfc_scan_id_task", 4096, NULL, 1, NULL);
    xTaskCreate(keypad_enter_id_task, "keypad_enter_id_task", 4096, NULL, 1, NULL);
    xTaskCreate(validation_task, "Validation Task", 4096, NULL, 1, NULL);
    xTaskCreate(display_task, "Display Task", 2048, NULL, 1, NULL);
    xTaskCreate(example_lvgl_port_task, "LVGL", EXAMPLE_LVGL_TASK_STACK_SIZE, NULL, EXAMPLE_LVGL_TASK_PRIORITY, &lvgl_port_task_handle);

    clear_buffer();

    xEventGroupSetBits(event_group, IDLE_BIT);

    _lock_acquire(&lvgl_api_lock);
    disp_obj = display_idle(display);
    _lock_release(&lvgl_api_lock);

    while (1)
    {
        switch (current_state)
        {
        case STATE_IDLE: // Wait until proximity is detected
            xEventGroupWaitBits(event_group, ENTERING_ID_BIT, pdTRUE, pdFALSE, portMAX_DELAY);
            current_state = STATE_USER_DETECTED;
            break;

        case STATE_USER_DETECTED: // Wait until NFC data is read or keypad press is entered
            xEventGroupWaitBits(event_group, ID_ENTERED_SUCCESS_BIT, pdTRUE, pdFALSE, portMAX_DELAY);
            current_state = STATE_VALIDATING;
            break;

        case STATE_VALIDATING: // Wait until validation is complete
            xEventGroupWaitBits(event_group, ID_AUTHENTICATED_BIT, pdTRUE, pdFALSE, portMAX_DELAY);
            current_state = STATE_DISPLAY_RESULT;
            break;

        case STATE_DISPLAY_RESULT:           // Result is displayed; transition back to idle state after showing result
            vTaskDelay(pdMS_TO_TICKS(5000)); // Display result for 5 seconds
            xEventGroupSetBits(event_group, IDLE_BIT);
            current_state = STATE_IDLE;
            break;
        default:
#ifdef MAIN_DEBUG
            ESP_LOGI(TAG, "Unknown state encountered: %d", current_state);
#endif
            current_state = STATE_IDLE;
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(100)); // Small delay to prevent rapid state change
    }
    ESP_LOGI(TAG, "Kiosk Application Finished");
}

// // Function to control state transitions and task management
// void state_control_task(void *pvParameter)
// {
//     while (1)
//     {
//         // ESP_LOGI("Memory", "Free heap size: %lu bytes", (unsigned long)esp_get_free_heap_size());

//         switch (current_state)
//         {
//         case STATE_WIFI_INIT:
//             // Start Wi-Fi init task if not already started
//             if (wifi_init_task_handle == NULL)
//             {
//                 ESP_LOGI(TAG, "Starting Wi-Fi Init Task");
//                 xTaskCreate(wifi_init_task, "wifi_init_task", 4096, NULL, 4, &wifi_init_task_handle);
//             }

//             // Start LED blinking task if not already running
//             if (blink_led_task_handle == NULL)
//             {
//                 ESP_LOGI(TAG, "Starting Blink LED Task");
//                 xTaskCreate(blink_led_task, "blink_led_task", 1024, NULL, 2, &blink_led_task_handle);
//             }

//             // Check if Wi-Fi init is completed (signaled by semaphore)
//             if (xSemaphoreTake(wifi_init_semaphore, portMAX_DELAY) == pdTRUE)
//             {
//                 current_state = STATE_WIFI_READY; // Transition state outside of the task
//             }

//             break;

//         case STATE_WIFI_READY:
//             // Stop Wi-Fi task when ready
//             if (wifi_init_task_handle != NULL)
//             {
//                 vTaskDelete(wifi_init_task_handle);
//                 wifi_init_task_handle = NULL;
//             }
//             // ESP_LOGI(TAG, "Wi-Fi Initialized. Ready!");

//             // if (blink_led_task_handle != NULL) {
//             //     vTaskDelete(blink_led_task_handle);
//             //     blink_led_task_handle = NULL;
//             // }

//             if (ota_update_task_handle == NULL)
//             {
//                 ESP_LOGI(TAG, "Creating OTA update task");
//                 xTaskCreate(ota_update_fw_task, "OTA UPDATE TASK", 1024 * 4, NULL, 8, &ota_update_task_handle);
//             }

//             break;

//         case STATE_ERROR:
//             // Handle error state - for now just stopping all tasks
//             if (wifi_init_task_handle != NULL)
//             {
//                 vTaskDelete(wifi_init_task_handle);
//                 wifi_init_task_handle = NULL;
//             }
//             if (blink_led_task_handle != NULL)
//             {
//                 vTaskDelete(blink_led_task_handle);
//                 blink_led_task_handle = NULL;
//             }
//             ESP_LOGE(TAG, "Error state reached!");
//             break;

//         default:
//             ESP_LOGW(TAG, "Unknown state encountered: %d", current_state);
//             break;
//         }
//         vTaskDelay(500 / portTICK_PERIOD_MS);
//     }
//     ESP_LOGI(TAG, "State control task finished"); // Should not reach here unless task is deleted
// }

// TaskHandle_t lvgl_port_task_handle = NULL;

// void app_main(void)
// {
//     ESP_LOGI(TAG, "App Main Start");

//     lv_display_t *display = gc9a01_init();

//     ESP_LOGI(TAG, "Create LVGL task");
//     xTaskCreate(example_lvgl_port_task, "LVGL", EXAMPLE_LVGL_TASK_STACK_SIZE, NULL, EXAMPLE_LVGL_TASK_PRIORITY, &lvgl_port_task_handle);

//     ESP_LOGI(TAG, "Display LVGL Meter Widget");
//     lv_obj_t *disp_obj;

//     _lock_acquire(&lvgl_api_lock);
//     disp_obj = display_idle(display);
//     _lock_release(&lvgl_api_lock);
//     vTaskDelay(5000 / portTICK_PERIOD_MS);
//     lv_obj_delete(disp_obj);

//     _lock_acquire(&lvgl_api_lock);
//     disp_obj = display_check_in_failed(display);
//     _lock_release(&lvgl_api_lock);
//     vTaskDelay(5000 / portTICK_PERIOD_MS);
//     lv_obj_delete(disp_obj);

//     _lock_acquire(&lvgl_api_lock);
//     disp_obj = display_check_in_success(display);
//     _lock_release(&lvgl_api_lock);
//     vTaskDelay(5000 / portTICK_PERIOD_MS);
//     lv_obj_delete(disp_obj);

//     _lock_acquire(&lvgl_api_lock);
//     disp_obj = display_transmitting(display);
//     _lock_release(&lvgl_api_lock);
//     vTaskDelay(5000 / portTICK_PERIOD_MS);

//     vTaskDelete(lvgl_port_task_handle);
//     lvgl_port_task_handle = NULL;

//     ESP_LOGI(TAG, "App Main End");

// // Initialize NVS
// esp_err_t ret = nvs_flash_init();
// if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
// {
//     // 1.OTA app partition table has a smaller NVS partition size than the non-OTA
//     // partition table. This size mismatch may cause NVS initialization to fail.
//     // 2.NVS partition contains data in new format and cannot be recognized by this version of code.
//     // If this happens, we erase NVS partition and initialize NVS again.
//     ESP_ERROR_CHECK(nvs_flash_erase());
//     ret = nvs_flash_init();
// }
// ESP_ERROR_CHECK(ret);
// // ESP_ERROR_CHECK( heap_trace_init_standalone( trace_record, NUM_RECORDS ) );
// // ESP_LOGI("Memory", "STARTING FREE HEAP SIZE: %lu bytes", (long unsigned int)esp_get_free_heap_size());
// /* Configure the peripheral according to the LED type */
// configure_led();

// // Create semaphore for signaling Wi-Fi init completion
// wifi_init_semaphore = xSemaphoreCreateBinary();

// xTaskCreate(state_control_task, "state_control_task", 4096 * 2, NULL, 3, NULL);

// while (1)
// {
//     vTaskDelay(4000 / portTICK_PERIOD_MS);
// }
// }