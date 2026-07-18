#include "mcu_io.h"
#include "io.h"
#include <avr/io.h>
#include <stdint.h>
#include <stdlib.h>

#define GET_SHADOW(CTX)   ((CTX)->pin < 8 ? &PORTB_shadow : ((CTX)->pin < 16 ? &PORTC_shadow : &PORTD_shadow))

#define MIO_INPUT         0
#define MIO_OUTPUT        1
#define MIO_HIGH          1
#define MIO_LOW           0  


typedef struct {
  uint8_t OLAT;
  uint8_t DDR;

  uint8_t OLAT_r;
  uint8_t PIN_r;
  uint8_t DDR_r;

  uint8_t port_id;
} MIOShadow;

static uint8_t mio_write(const IOContext* _ctx, IOLevel level);
static uint8_t mio_read(const IOContext* _ctx, IOLevel* level);
static uint8_t mio_read_olat(const IOContext* _ctx, IOLevel* level);
static uint8_t mio_highz(const IOContext* _ctx);

MIOShadow PORTB_shadow = { .port_id = 0 };
MIOShadow PORTC_shadow = { .port_id = 1 };
MIOShadow PORTD_shadow = { .port_id = 2 };

const IOVtable mio_vtable = {
  .write = &mio_write,
  .read = &mio_read,
  .read_olat = &mio_read_olat,
  .highz = &mio_highz
};

uint8_t mio_init(void) {
  const uint8_t DDR = MIO_INPUT == 0 ? 0x00 : 0xFF;

  PORTB_shadow.DDR = DDR;
  PORTC_shadow.DDR = DDR;
  PORTD_shadow.DDR = DDR;

  if (mio_flush()) return 1;

  return 0;
}

uint8_t mio_flush(void) {
#if MIO_OUTPUT != 1 || MIO_HIGH != 1
#error "This function's logic needs to be adjusted because expected values are no longer given"
#endif

  DDRB = PORTB_shadow.DDR;
  DDRC = PORTC_shadow.DDR;
  DDRD = PORTD_shadow.DDR;

  PORTB = PORTB_shadow.OLAT & PORTB_shadow.DDR;
  PORTC = PORTC_shadow.OLAT & PORTC_shadow.DDR;
  PORTD = PORTD_shadow.OLAT & PORTD_shadow.DDR;

  PORTB_shadow.OLAT_r = PORTB_shadow.OLAT;
  PORTC_shadow.OLAT_r = PORTC_shadow.OLAT;
  PORTD_shadow.OLAT_r = PORTD_shadow.OLAT;

  PORTB_shadow.PIN_r = PINB;
  PORTC_shadow.PIN_r = PINC;
  PORTD_shadow.PIN_r = PIND;

  return 0;
}

static uint8_t mio_write_iodir(const IOContext* _ctx, IOMode mode) {
  const MIOContext* ctx = _ctx;

  if (ctx == NULL) return 1;
  if (ctx->pin > 23) return 1;
  if (mode != iom_INPUT && mode != iom_OUTPUT) return 1;

  MIOShadow* shadow = GET_SHADOW(ctx);

  shadow->DDR &= ~(1 << (ctx->pin % 8));
  shadow->DDR |= ((mode == iom_OUTPUT) ? MIO_OUTPUT : MIO_INPUT) << (ctx->pin % 8);

  return 0;
}

static uint8_t mio_read_iodir(const IOContext* _ctx, IOMode* mode) {
  const MIOContext* ctx = _ctx;

  if (ctx == NULL) return 1;
  if (ctx->pin > 23) return 1;
  if (mode == NULL) return 1;

  MIOShadow* shadow = GET_SHADOW(ctx);

  *mode = (shadow->DDR_r >> (ctx->pin % 8) & 1) == MIO_OUTPUT ? iom_OUTPUT : iom_INPUT;

  return 0;
}

static uint8_t mio_write(const IOContext* _ctx, IOLevel level) {
  const MIOContext* ctx = _ctx;

  if (ctx == NULL) return 1;
  if (ctx->pin > 23) return 1;
  if (level != iol_LOW && level != iol_HIGH) return 1;

  if (mio_write_iodir(ctx, iom_OUTPUT)) return 1;

  MIOShadow* shadow = GET_SHADOW(ctx);

  shadow->OLAT &= ~(1 << (ctx->pin % 8));
  shadow->OLAT |= ((level == iol_HIGH) ? MIO_HIGH : MIO_LOW) << (ctx->pin % 8);

  return 0;
}

static uint8_t mio_read(const IOContext* _ctx, IOLevel* level) {
  const MIOContext* ctx = _ctx;

  if (ctx == NULL) return 1;
  if (ctx->pin > 23) return 1;
  if (level == NULL) return 1;
  
  IOMode mode;
  if (mio_read_iodir(ctx, &mode)) return 1;
  if (mode != iom_INPUT) return 1;

  MIOShadow* shadow = GET_SHADOW(ctx);

  *level = (shadow->PIN_r >> (ctx->pin % 8) & 1) == MIO_HIGH ? iol_HIGH : iol_LOW;

  return 0;
}

static uint8_t mio_read_olat(const IOContext* _ctx, IOLevel* level) {
  const MIOContext* ctx = _ctx;

  if (ctx == NULL) return 1;
  if (ctx->pin > 23) return 1;
  if (level == NULL) return 1;
  
  IOMode mode;
  if (mio_read_iodir(ctx, &mode)) return 1;
  if (mode != iom_OUTPUT) return 1;

  MIOShadow* shadow = GET_SHADOW(ctx);

  *level = (shadow->OLAT_r >> (ctx->pin % 8) & 1) == MIO_HIGH ? iol_HIGH : iol_LOW;

  return 0;
}

static uint8_t mio_highz(const IOContext* _ctx) {
  const MIOContext* ctx = _ctx;

  if (ctx == NULL) return 1;
  if (ctx->pin > 23) return 1;

  if (mio_write_iodir(ctx, MIO_INPUT)) return 1;

  return 0;
}

