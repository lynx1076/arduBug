#ifndef IO_H
#define IO_H

#include <stdint.h>

typedef enum {
  io_UNKNOWN,
  io_HIGH,
  io_LOW, 
} IOState;

typedef enum {
  iom_UNKNOWN,
  iom_INPUT,
  iom_OUTPUT
} IOMode;

typedef struct {
  const char* name;
  const uint8_t id;
  const uint8_t ctx_id;

  IOState state;
  IOMode mode;
} IOPin;

uint8_t io_write(IOPin* pin, IOMode mode, IOState state);
uint8_t io_read(IOPin* pin, IOMode* mode, IOState* state);

#endif

