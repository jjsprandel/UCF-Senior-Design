idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "." "./include"
    REQUIRES "display" "esp_driver_gpio" "pn532" "keypad_driver" "NDEF" # "wifi"
    PRIV_REQUIRES "nvs_flash"
    )
