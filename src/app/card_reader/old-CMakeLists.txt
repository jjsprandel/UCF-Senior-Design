idf_component_register(SRCS "card_reader.c"
    INCLUDE_DIRS "."
    REQUIRES "esp_driver_gpio" "pn532")