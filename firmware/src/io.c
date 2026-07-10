#include "io.h"
#include <stdint.h>


uint8_t io_write(IOPin* pin, IOState state) {
  if (!pin) return 1;
  if (pin->vtable->write(pin->context, state)) return 1;

  return 0;
}

uint8_t io_read(IOPin* pin, IOState* state) {
  if (!pin) return 1;
  if (pin->vtable->read(pin->context, state)) return 1;

  return 0;
}

uint8_t io_read_olat(IOPin* pin, IOState* state) {
  if (!pin) return 1;
  if (pin->vtable->read_olat(pin->context, state)) return 1;

  return 0;
}

uint8_t io_highz(IOPin* pin) {
  if (!pin) return 1;
  if (pin->vtable->highz(pin->context)) return 1;

  return 0;
}

