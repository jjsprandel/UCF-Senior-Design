idf_component_register(SRCS "main.c"
    INCLUDE_DIRS "." "./include"
    REQUIRES wifi card_reader pn532 esp_driver_uart esp_driver_gpio keypad_driver unity
    PRIV_REQUIRES nvs_flash)
