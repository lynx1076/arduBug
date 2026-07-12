#include "mcp23017.h"
#include "twi.h"
#include "io.h"
#include <stdint.h>

#define MCP_ADDR_IO0      0x20
#define MCP_ADDR_IO1      0x21

#define MCP_IODIRA        0x00
#define MCP_IODIRB        0x01

#define MCP_GPIOA         0x12
#define MCP_GPIOB         0x13

#define MCP_OLATA         0x14
#define MCP_OLATB         0x15

#define MCP_PULLUPA       0x0C
#define MCP_PULLUPB       0x0D

#define MCP_HIGH        1
#define MCP_LOW         0
#define MCP_OUTPUT      0
#define MCP_INPUT       1

#define GET_ADDR(CTX) ((CTX)->pin < 16 ? MCP_ADDR_IO0 : MCP_ADDR_IO1)
#define GET_AB(CTX) ((CTX)->pin % 16 < 8)
#define GET_IODIR(CTX) (GET_AB(CTX) ? MCP_IODIRA : MCP_IODIRB)
#define GET_GPIO(CTX) (GET_AB(CTX) ? MCP_GPIOA : MCP_GPIOB)
#define GET_OLAT(CTX) (GET_AB(CTX) ? MCP_OLATA : MCP_OLATB)

uint8_t mcp_write_pin_iodir(const MCPContext* ctx, IOMode mode);
uint8_t mcp_write_pin(const MCPContext* ctx, IOLevel state);
uint8_t mcp_read_pin(const MCPContext* ctx, IOLevel* state);
uint8_t mcp_read_olat(const MCPContext* ctx, IOLevel* state);
uint8_t mcp_highz(const MCPContext* ctx);

uint8_t mcp_write_pin_iodir_w(const IOContext* ctx, IOMode mode);
uint8_t mcp_write_pin_w(const IOContext* ctx, IOLevel state);
uint8_t mcp_read_pin_w(const IOContext* ctx, IOLevel* state);
uint8_t mcp_read_olat_w(const IOContext* ctx, IOLevel* state);
uint8_t mcp_highz_w(const IOContext* ctx);

const IOVtable mcp_vtable = {
  .write = &mcp_write_pin_w,
  .read = &mcp_read_pin_w,
  .read_olat = &mcp_read_olat_w,
  .highz = &mcp_highz_w
};

uint8_t mcp_write_pin_iodir(const MCPContext* ctx, IOMode mode) {
  if (!ctx) return 1;
  if (ctx->pin > 31) return 1;
  if (mode != iom_INPUT && mode != iom_OUTPUT) return 1;

  uint8_t addr = GET_ADDR(ctx);
  uint8_t reg = GET_IODIR(ctx);

  uint8_t port;
  if (twi_read_reg(addr, reg, &port)) return 1;

  uint8_t rel_pin = ctx->pin % 8;
 

  port = (port & ~(1 << rel_pin)) | ((mode == iom_OUTPUT ? MCP_OUTPUT : MCP_INPUT) << rel_pin);

  if (twi_write_reg(addr, reg, port)) return 1;

  if (mode == iom_INPUT) {
    uint8_t pull_reg = GET_AB(ctx) ? MCP_PULLUPA : MCP_PULLUPB;
    uint8_t pull;
    if (twi_read_reg(addr, pull_reg, &pull)) return 1;
    pull &= ~(1 << rel_pin);
    if (twi_write_reg(addr, pull_reg, pull)) return 1;
  }

  return 0;
}

uint8_t mcp_write_pin(const MCPContext* ctx, IOLevel state) {
  if (!ctx) return 1;
  if (ctx->pin > 31) return 1;

  uint8_t addr = GET_ADDR(ctx);
  uint8_t reg = GET_GPIO(ctx);

  uint8_t port;
  if (twi_read_reg(addr, reg, &port)) return 1;

  if (mcp_write_pin_iodir(ctx, iom_OUTPUT)) return 1;

  uint8_t rel_pin = ctx->pin % 8;
 
  port = (port & ~(1 << rel_pin)) | ((state == io_HIGH ? MCP_HIGH : MCP_LOW) << rel_pin);

  if (twi_write_reg(addr, reg, port)) return 1;

  return 0;
}

uint8_t mcp_read_pin(const MCPContext* ctx, IOLevel* state) {
  if (!ctx) return 1;
  if (ctx->pin > 31) return 1;

  if (mcp_write_pin_iodir(ctx, iom_INPUT)) return 1;

  uint8_t addr = GET_ADDR(ctx);
  uint8_t reg = GET_GPIO(ctx);

  uint8_t port;
  if (twi_read_reg(addr, reg, &port)) return 1;

  uint8_t rel_pin = ctx->pin % 8;
 
  *state = ((port >> rel_pin) & 1) == MCP_HIGH ? io_HIGH : io_LOW;

  return 0;
}

uint8_t mcp_read_olat(const MCPContext* ctx, IOLevel* state) {
  if (!ctx) return 1;
  if (ctx->pin > 31) return 1;

  uint8_t addr = GET_ADDR(ctx);
  uint8_t reg = GET_OLAT(ctx);

  if (mcp_write_pin_iodir(ctx, iom_OUTPUT)) return 1;

  uint8_t port;
  if (twi_read_reg(addr, reg, &port)) return 1;

  uint8_t rel_pin = ctx->pin % 8;
 
  *state = ((port >> rel_pin) & 1) == MCP_HIGH ? io_HIGH : io_LOW;

  return 0;
}

uint8_t mcp_highz(const MCPContext* ctx) {
  if (!ctx) return 1;

  if (mcp_write_pin_iodir(ctx, iom_INPUT)) return 1;

  return 0;
}

uint8_t mcp_write_pin_iodir_w(const IOContext* ctx, IOMode mode) {
  return mcp_write_pin_iodir((const MCPContext*)ctx, mode);
}

uint8_t mcp_write_pin_w(const IOContext* ctx, IOLevel state) {
  return mcp_write_pin((const MCPContext*)ctx, state);
}

uint8_t mcp_read_pin_w(const IOContext* ctx, IOLevel* state) {
  return mcp_read_pin((const MCPContext*)ctx, state);
}

uint8_t mcp_read_olat_w(const IOContext* ctx, IOLevel* state) {
  return mcp_read_olat((const MCPContext*)ctx, state);
}

uint8_t mcp_highz_w(const IOContext* ctx) {
  return mcp_highz((const MCPContext*)ctx);
}

