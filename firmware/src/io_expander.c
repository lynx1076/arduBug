#include "io_expander.h"
#include "twi.h"
#include "io.h"
#include <stdlib.h>
#include <stdint.h>

#define IOX_ADDR_0      0x20
#define IOX_ADDR_1      0x21

#define IOX_IODIRA      0x00
#define IOX_IODIRB      0x01

#define IOX_GPIOA       0x12
#define IOX_GPIOB       0x13

#define IOX_OLATA       0x14
#define IOX_OLATB       0x15

#define IOX_PULLUPA     0x0C
#define IOX_PULLUPB     0x0D

#define IOX_HIGH        1
#define IOX_LOW         0
#define IOX_OUTPUT      0
#define IOX_INPUT       1

#define IS_IOX_0(CTX)     ((CTX)->pin < 16)
#define IS_PORT_A(CTX)    ((CTX)->pin % 16 < 8)

typedef struct {
  uint8_t addr;

  uint8_t OLATA, OLATB;
  uint8_t IODIRA, IODIRB;

  uint8_t GPIOA_r, GPIOB_r;
  uint8_t OLATA_r, OLATB_r;
  uint8_t IODIRA_r, IODIRB_r;
} IOXShadow;

static uint8_t iox_flush_shadow(IOXShadow* shadow);
static uint8_t iox_write_iodir(const IOContext* _ctx, IOMode mode);
static uint8_t iox_read_iodir(const IOContext* _ctx, IOMode* mode);
static uint8_t iox_write(const IOContext* _ctx, IOLevel level);
static uint8_t iox_read(const IOContext* _ctx, IOLevel* level);
static uint8_t iox_read_olat(const IOContext* _ctx, IOLevel* level);
static uint8_t iox_highz(const IOContext* _ctx);

static IOXShadow iox0_shadow = { .addr = IOX_ADDR_0 };
static IOXShadow iox1_shadow = { .addr = IOX_ADDR_1 };

const IOVtable iox_vtable = {
  .write = &iox_write,
  .read = &iox_read,
  .read_olat = &iox_read_olat,
  .highz = &iox_highz
};

uint8_t iox_init(void) {
  const uint8_t IODIR = IOX_INPUT == 0 ? 0x00 : 0xFF;

  iox0_shadow.IODIRA = IODIR;
  iox0_shadow.IODIRB = IODIR;
  iox1_shadow.IODIRA = IODIR;
  iox1_shadow.IODIRB = IODIR;

  if (iox_flush()) return 1;

  return 0;
}

static uint8_t iox_flush_shadow(IOXShadow* shadow) {
  if (twi_write_reg(shadow->addr, IOX_IODIRA, shadow->IODIRA)) return 1;
  shadow->IODIRA_r = shadow->IODIRA;
  if (twi_write_reg(shadow->addr, IOX_IODIRB, shadow->IODIRB)) return 1;
  shadow->IODIRB_r = shadow->IODIRB;

  if (twi_write_reg(shadow->addr, IOX_OLATA, shadow->OLATA)) return 1;
  shadow->OLATA_r = shadow->OLATA;
  if (twi_write_reg(shadow->addr, IOX_OLATB, shadow->OLATB)) return 1;
  shadow->OLATB_r = shadow->OLATB;

  if (twi_read_reg(shadow->addr, IOX_GPIOA, &shadow->GPIOA_r)) return 1;
  if (twi_read_reg(shadow->addr, IOX_GPIOB, &shadow->GPIOB_r)) return 1;

  return 0;
}

uint8_t iox_flush(void) {
  if (iox_flush_shadow(&iox0_shadow)) return 1;
  if (iox_flush_shadow(&iox1_shadow)) return 1;

  return 0;
}

static uint8_t iox_write_iodir(const IOContext* _ctx, IOMode mode) {
  const IOXContext* ctx = _ctx;

  if (ctx == NULL) return 1;
  if (mode != iom_INPUT && mode != iom_OUTPUT) return 1;

  IOXShadow* shadow = IS_IOX_0(ctx) ? &iox0_shadow : &iox1_shadow;
  uint8_t* reg = IS_PORT_A(ctx) ? &shadow->IODIRA : &shadow->IODIRB;

  *reg &= ~(1 << (ctx->pin % 8));
  *reg |= ((mode == iom_OUTPUT) ? IOX_OUTPUT : IOX_INPUT) << (ctx->pin % 8);

  return 0;
}

static uint8_t iox_read_iodir(const IOContext* _ctx, IOMode* mode) {
  const IOXContext* ctx = _ctx;

  if (ctx == NULL) return 1;
  if (mode == NULL) return 1;

  IOXShadow* shadow = IS_IOX_0(ctx) ? &iox0_shadow : &iox1_shadow;
  uint8_t* reg = IS_PORT_A(ctx) ? &shadow->IODIRA : &shadow->IODIRB;

  *mode = ((*reg >> (ctx->pin % 8)) & 1) == IOX_OUTPUT ? iom_OUTPUT : iom_INPUT;

  return 0;
}

static uint8_t iox_write(const IOContext* _ctx, IOLevel level) {
  const IOXContext* ctx = _ctx;

  if (ctx == NULL) return 1;
  if (level != iol_LOW && level != iol_HIGH) return 1;
  if (iox_write_iodir(ctx, iom_OUTPUT)) return 1;

  IOXShadow* shadow = IS_IOX_0(ctx) ? &iox0_shadow : &iox1_shadow;
  uint8_t* reg = IS_PORT_A(ctx) ? &shadow->OLATA : &shadow->OLATB;

  *reg &= ~(1 << (ctx->pin % 8));
  *reg |= ((level == iol_HIGH) ? IOX_HIGH : IOX_LOW) << (ctx->pin % 8);

  return 0;
}

static uint8_t iox_read(const IOContext* _ctx, IOLevel* level) {
  const IOXContext* ctx = _ctx;

  if (ctx == NULL) return 1;
  if (level == NULL) return 1;

  IOMode mode;
  if (iox_read_iodir(ctx, &mode)) return 1;
  if (mode != iom_INPUT) return 1;

  IOXShadow* shadow = IS_IOX_0(ctx) ? &iox0_shadow : &iox1_shadow;
  uint8_t* reg = IS_PORT_A(ctx) ? &shadow->GPIOA_r : &shadow->GPIOB_r;

  *level = ((*reg >> (ctx->pin % 8)) & 1) == IOX_HIGH ? iol_HIGH : iol_LOW;

  return 0;
}

static uint8_t iox_read_olat(const IOContext* _ctx, IOLevel* level) {
  const IOXContext* ctx = _ctx;

  if (ctx == NULL) return 1;
  if (level == NULL) return 1;

  IOMode mode;
  if (iox_read_iodir(ctx, &mode)) return 1;
  if (mode != iom_OUTPUT) return 1;

  IOXShadow* shadow = IS_IOX_0(ctx) ? &iox0_shadow : &iox1_shadow;
  uint8_t* reg = IS_PORT_A(ctx) ? &shadow->OLATA_r : &shadow->OLATB_r;

  *level = ((*reg >> (ctx->pin % 8)) & 1) == IOX_HIGH ? iol_HIGH : iol_LOW;

  return 0;
}

static uint8_t iox_highz(const IOContext* _ctx) {
  const IOXContext* ctx = _ctx;

  if (ctx == NULL) return 1;

  if (iox_write_iodir(ctx, iom_INPUT)) return 1;

  return 0;
}

