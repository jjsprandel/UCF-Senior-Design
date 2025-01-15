idf_component_register(SRCS "display-frames.c"
                    INCLUDE_DIRS "."
                    REQUIRES esp_wifi
                    PRIV_REQUIRES nvs_flash)