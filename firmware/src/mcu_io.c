#include "mcu_io.h"
#include "io.h"
#include <avr/io.h>

#define GET_PORT(pin) ((pin) < 8 ? 0 : ((pin) < 15 ? 1 : 2))
#define GET_SHIFT(pin) ((pin) < 8 ? (pin) : ((pin) < 15 ? ((pin) - 8) : ((pin) - 15)))

const IOVtable mio_vtable = {
  .write = mio_write_pin_w,
  .read = mio_read_pin_w,
  .read_olat = mio_read_olat_w,
  .highz = mio_highz_w
};

uint8_t mio_write_pin_iodir(MIOContext* ctx, IOMode mode) {
  if (!ctx) return 1;

  uint8_t port = GET_PORT(ctx->pin);
  uint8_t shift = GET_SHIFT(ctx->pin);

  if (mode == iom_OUTPUT) {
    if (port == 0) DDRB |= (1 << shift);
    else if (port == 1) DDRC |= (1 << shift);
    else DDRD |= (1 << shift);
  } else {
    if (port == 0) DDRB &= ~(1 << shift);
    else if (port == 1) DDRC &= ~(1 << shift);
    else DDRD &= ~(1 << shift);
  }

  return 0;
}

uint8_t mio_write_pin(MIOContext* ctx, IOState state) {
  if (!ctx) return 1;

  uint8_t port = GET_PORT(ctx->pin);
  uint8_t shift = GET_SHIFT(ctx->pin);

  if (state == io_HIGH) {
    if (port == 0) PORTB |= (1 << shift);
    else if (port == 1) PORTC |= (1 << shift);
    else PORTD |= (1 << shift);
  } else {
    if (port == 0) PORTB &= ~(1 << shift);
    else if (port == 1) PORTC &= ~(1 << shift);
    else PORTD &= ~(1 << shift);
  }

  return 0;
}

uint8_t mio_read_pin(MIOContext* ctx, IOState* state) {
  if (!ctx || !state) return 1;

  uint8_t port = GET_PORT(ctx->pin);
  uint8_t shift = GET_SHIFT(ctx->pin);
  uint8_t bit_sample = 0;

  if (port == 0) bit_sample = PINB & (1 << shift);
  else if (port == 1) bit_sample = PINC & (1 << shift);
  else bit_sample = PIND & (1 << shift);

  *state = bit_sample ? io_HIGH : io_LOW;
  return 0;
}

uint8_t mio_read_olat(MIOContext* ctx, IOState* state) {
  if (!ctx || !state) return 1;

  uint8_t port = GET_PORT(ctx->pin);
  uint8_t shift = GET_SHIFT(ctx->pin);
  uint8_t bit_sample = 0;

  if (port == 0) bit_sample = PORTB & (1 << shift);
  else if (port == 1) bit_sample = PORTC & (1 << shift);
  else bit_sample = PORTD & (1 << shift);

  *state = bit_sample ? io_HIGH : io_LOW;
  return 0;
}

uint8_t mio_highz(MIOContext* ctx) {
  if (!ctx) return 1;

  if (mio_write_pin_iodir(ctx, iom_INPUT)) return 1;

  return 0;
}

uint8_t mio_write_pin_iodir_w(IOContext* ctx, IOMode mode) {
  return mio_write_pin_iodir((MIOContext*)ctx, mode);
}

uint8_t mio_write_pin_w(IOContext* ctx, IOState state) {
  return mio_write_pin((MIOContext*)ctx, state);
}

uint8_t mio_read_pin_w(IOContext* ctx, IOState* state) {
  return mio_read_pin((MIOContext*)ctx, state);
}

uint8_t mio_read_olat_w(IOContext* ctx, IOState* state) {
  return mio_read_olat((MIOContext*)ctx, state);
}

uint8_t mio_highz_w(IOContext* ctx) {
  return mio_highz((MIOContext*)ctx);
}

