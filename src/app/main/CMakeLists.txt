idf_component_register(SRCS "main.c"
    INCLUDE_DIRS "." "./include"
    REQUIRES "wifi" 
    "NDEF" 
    "esp_driver_uart" 
    "esp_driver_gpio")
