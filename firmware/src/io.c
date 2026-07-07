#include "io.h"
#include "mcp23017.h"
#include "pins.h"
#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <stdint.h>


uint8_t io_write(IOPin* pin, IOState state) {
  return 0;
}

uint8_t io_read(IOPin* pin, IOState* state) {
  return 0;
}

