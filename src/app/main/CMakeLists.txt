idf_component_register(SRCS "main.c"
    INCLUDE_DIRS "." "./include"
    REQUIRES wifi pn532 esp_driver_uart esp_driver_gpio keypad_driver unity NDEF
    PRIV_REQUIRES nvs_flash)
