idf_component_register(SRCS "wifi_init.c"
                    INCLUDE_DIRS "."
                    REQUIRES esp_wifi
                    PRIV_REQUIRES nvs_flash)