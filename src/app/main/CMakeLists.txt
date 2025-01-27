idf_component_register(SRCS "main.c"
    INCLUDE_DIRS "." "./include"
    REQUIRES wifi NDEF pn532 esp_driver_uart esp_driver_gpio keypad_driver unity
    PRIV_REQUIRES nvs_flash)
