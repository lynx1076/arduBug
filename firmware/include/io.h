#ifndef IO_H
#define IO_H

#include <stdint.h>

typedef enum {
  io_UNDEFINED = -1,
  io_HIGH,
  io_LOW, 
} IOLevel;

typedef enum {
  iom_UNDEFINED = -1,
  iom_INPUT,
  iom_OUTPUT
} IOMode;

typedef void IOContext;

typedef struct {
  uint8_t (*write)(const IOContext* ctx, IOLevel writeState);
  uint8_t (*read)(const IOContext* ctx, IOLevel* readState);
  uint8_t (*read_olat)(const IOContext* ctx, IOLevel* readState);
  uint8_t (*highz)(const IOContext* ctx);
} IOVtable;

typedef struct {
  const char* name;
  const uint8_t id;

  IOMode mode;
  IOLevel level;

  const IOVtable* vtable;
  const IOContext* context;
} IOPin;

uint8_t io_write(IOPin* pin, IOLevel state);
uint8_t io_read(IOPin* pin, IOLevel* state);
uint8_t io_read_olat(IOPin* pin, IOLevel* state);
uint8_t io_highz(IOPin* pin);

#endif

