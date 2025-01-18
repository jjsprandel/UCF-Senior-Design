idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS "." "./include" "../firebase/include"
                    REQUIRES "wifi" "esp_driver_gpio" "firebase")
