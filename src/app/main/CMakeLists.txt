idf_component_register(SRCS "main.c"
    INCLUDE_DIRS "." "./include"
    REQUIRES "wifi" 
    "pn532" 
    "card_reader" 
    "esp_driver_uart" 
    "esp_driver_gpio")
