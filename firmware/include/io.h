#ifndef IO_H
#define IO_H

#include <stdint.h>

typedef enum {
  io_UNKNOWN = -1,
  io_HIGH,
  io_LOW, 
} IOState;

typedef enum {
  iom_UNKNOWN = -1,
  iom_INPUT,
  iom_OUTPUT
} IOMode;

typedef void IOContext;

typedef struct {
  uint8_t (*write)(IOContext* ctx, IOState writeState);
  uint8_t (*read)(IOContext* ctx, IOState* readState);
  uint8_t (*read_olat)(IOContext* ctx, IOState* readState);
  uint8_t (*highz)(IOContext* ctx);
} IOVtable;

typedef struct {
  const char* name;
  const uint8_t id;

  const IOVtable* vtable;
  IOContext* context;
} IOPin;

uint8_t io_write(IOPin* pin, IOState state);
uint8_t io_read(IOPin* pin, IOState* state);
uint8_t io_read_olat(IOPin* pin, IOState* state);
uint8_t io_highz(IOPin* pin);

#endif

