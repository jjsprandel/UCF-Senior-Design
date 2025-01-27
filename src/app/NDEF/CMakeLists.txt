idf_component_register(SRCS "ndef_message.c" "ndef_record.c" "mifare_classic.c" "nfc_adapter.c"
                    INCLUDE_DIRS "."
                    REQUIRES esp_driver_gpio pn532)