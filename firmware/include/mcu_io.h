#ifndef MCU_IO_H
#define MCU_IO_H

#include "io.h"
#include <stdint.h>

uint8_t mio_write(uint8_t pin, PinState state);
uint8_t mio_setpullup(uint8_t pin, PinState state);
uint8_t mio_setmode(uint8_t pin, PinMode mode);
uint8_t mio_read(uint8_t pin, PinState* state);

#endif

