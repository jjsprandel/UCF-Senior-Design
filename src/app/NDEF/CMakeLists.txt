idf_component_register(SRCS "ndef_message.c" "ndef_record.c" "nfc_tag.c" "card_reader.c"
                    INCLUDE_DIRS "."
                    REQUIRES esp_driver_gpio pn532)