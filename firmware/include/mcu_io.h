#ifndef MCU_IO_H
#define MCU_IO_H

#include "io.h"
#include <stdint.h>

#define MIO_PB0       0
#define MIO_PB1       1
#define MIO_PB2       2
#define MIO_PB3       3
#define MIO_PB4       4
#define MIO_PB5       5
#define MIO_PB6       6
#define MIO_PB7       7

#define MIO_PC0       8
#define MIO_PC1       9
#define MIO_PC2       10
#define MIO_PC3       11
#define MIO_PC4       12
#define MIO_PC5       13
#define MIO_PC6       14

#define MIO_PD0       15
#define MIO_PD1       16
#define MIO_PD2       17
#define MIO_PD3       18
#define MIO_PD4       19
#define MIO_PD5       20
#define MIO_PD6       21
#define MIO_PD7       22

typedef struct {
  uint8_t pin;
} MIOContext;

extern const IOVtable mio_vtable;

uint8_t mio_write_pin_iodir(MIOContext* ctx, IOMode mode);
uint8_t mio_write_pin(MIOContext* ctx, IOState state);
uint8_t mio_read_pin(MIOContext* ctx, IOState* state);
uint8_t mio_read_olat(MIOContext* ctx, IOState* state);
uint8_t mio_highz(MIOContext* ctx);

uint8_t mio_write_pin_iodir_w(IOContext* ctx, IOMode mode);
uint8_t mio_write_pin_w(IOContext* ctx, IOState state);
uint8_t mio_read_pin_w(IOContext* ctx, IOState* state);
uint8_t mio_read_olat_w(IOContext* ctx, IOState* state);
uint8_t mio_highz_w(IOContext* ctx);

#endif
