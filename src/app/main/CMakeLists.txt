idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS "." "./include"
                    REQUIRES "wifi" "nfc_init" "esp_driver_gpio")
