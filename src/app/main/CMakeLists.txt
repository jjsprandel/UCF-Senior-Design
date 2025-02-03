idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "." "./include"
    REQUIRES "display" "esp_driver_gpio" #"wifi" "pn532"  "keypad_driver" "NDEF" 
    PRIV_REQUIRES "nvs_flash"
    )
