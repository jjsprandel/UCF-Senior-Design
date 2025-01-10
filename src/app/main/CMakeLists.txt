idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS "." "./include"
                    REQUIRES "wifi" "keypad" "driver" "esp_driver_gpio")
