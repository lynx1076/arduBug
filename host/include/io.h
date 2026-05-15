#ifndef IO_H
#define IO_H

#include "result.h"
#include <stdint.h>
#include <stdbool.h>

result io_get_pin_from_token(const char* token, uint8_t* pin);
result io_get_state_from_token(const char* token, bool* state);
result io_write_iodir(uint8_t pin, bool state);
result io_write_pin(uint8_t pin, bool state);
result io_get_output_pin_state(uint8_t pin, bool* state);
result io_read_pin(uint8_t pin, bool* state);

#endif
