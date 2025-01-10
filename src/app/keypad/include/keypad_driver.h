char poll_keypad(uint8_t keypad_address);

void init_timer(keypad_buffer_t *buffer);

void clear_buffer(keypad_buffer_t *buffer);

void add_to_buffer(keypad_buffer_t *buffer, char val);

void keypad_handler(void *params);
esp_err_t i2c_master_init(void);