idf_component_register(SRCS "main.c" "nfc_init.c"
                    INCLUDE_DIRS "."
                    REQUIRES "wifi" "pn532" "esp_driver_gpio")
