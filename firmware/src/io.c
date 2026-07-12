#include "io.h"
#include <stdint.h>


uint8_t io_write(IOPin* pin, IOLevel state) {
  if (!pin) return 1;
  if (state == io_UNDEFINED) return 1;

  if (pin->level == state && pin->mode == iom_OUTPUT) return 0;
  if (pin->vtable->write(pin->context, state)) return 1;

  pin->mode = iom_OUTPUT;
  pin->level = state;

  return 0;
}

uint8_t io_read(IOPin* pin, IOLevel* state) {
  if (!pin) return 1;

  if (pin->vtable->read(pin->context, state)) return 1;

  pin->mode = iom_INPUT;
  pin->level = io_UNDEFINED;

  return 0;
}

uint8_t io_read_olat(IOPin* pin, IOLevel* state) {
  if (!pin) return 1;

  if (pin->level != io_UNDEFINED && pin->mode == iom_OUTPUT) {
    *state = pin->level;
    return 0;
  }
  if (pin->vtable->read_olat(pin->context, state)) return 1;

  pin->mode = iom_OUTPUT;
  pin->level = *state;

  return 0;
}

uint8_t io_highz(IOPin* pin) {
  if (!pin) return 1;

  if (pin->mode == iom_INPUT) return 0;
  if (pin->vtable->highz(pin->context)) return 1;

  pin->mode = iom_INPUT;
  pin->level = io_UNDEFINED;

  return 0;
}

