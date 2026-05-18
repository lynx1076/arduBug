#ifndef IO_H
#define IO_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

int io_init(void);
int io_get_pin_from_token(const char* token, uint8_t* pin);
int io_get_state_from_token(const char* token, bool* state);
int io_write_iodir(uint8_t pin, bool state);
int io_write_pin(uint8_t pin, bool state);
int io_read_pin(uint8_t pin, bool* state);


int io_read_pin_output(uint8_t pin, bool* state);
int io_set_clock(bool phase);
int io_get_clock(bool* phase);
int io_pulse_clock(const size_t n);
int io_reset_seq(void);
int io_cpu_reading(bool* is_reading);
int io_read_databus(uint8_t* databus);
int io_read_addrbus(uint16_t* addrbus);
int io_write_databus(uint8_t databus);

#endif
