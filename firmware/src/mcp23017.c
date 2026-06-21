#include "mcp23017.h"
#include "pins.h"
#include "twi.h"
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

static uint8_t get_addr_from_pin(uint8_t pin, uint8_t* addr) {
  if (pin < 16) {
    *addr = MCP_ADDR_IO0;
    return 0;
  }

  if (pin >= 16 && pin < 32) {
    *addr = MCP_ADDR_IO1;
    return 0;
  }

  return 1;
}

static uint8_t get_gpio_from_pin(uint8_t pin, uint8_t* port) {
  if (pin / 16 >= 2) return 1;

  uint8_t rel_pin = pin % 16;

  if (rel_pin < 8) {
    *port = MCP_GPIOA;
    return 0;
  }

  if (rel_pin >= 8) {
    *port = MCP_GPIOB;
    return 0;
  }

  return 1;
}

static uint8_t get_iodir_from_pin(uint8_t pin, uint8_t* iodir) {
  if (pin / 16 >= 2) return 1;

  uint8_t rel_pin = pin % 16;

  if (rel_pin < 8) {
    *iodir = MCP_IODIRA;
    return 0;
  }

  if (rel_pin >= 8) {
    *iodir = MCP_IODIRB;
    return 0;
  }

  return 1;
}

static uint8_t get_pull_up_from_pin(uint8_t pin, uint8_t* pull_up) {
  if (pin / 16 >= 2) return 1;

  uint8_t rel_pin = pin % 16;

  if (rel_pin < 8) {
    *pull_up = MCP_PULLUPA;
    return 0;
  }

  if (rel_pin >= 8) {
    *pull_up = MCP_PULLUPB;
    return 0;
  }

  return 1;
}

static uint8_t get_olat_from_pin(uint8_t pin, uint8_t* olat) {
  if (pin / 16 >= 2) return 1;

  uint8_t rel_pin = pin % 16;

  if (rel_pin < 8) {
    *olat = MCP_OLATA;
    return 0;
  }

  if (rel_pin >= 8) {
    *olat = MCP_OLATB;
    return 0;
  }

  return 1;
}

uint8_t mcp_write_pin(uint8_t pin, uint8_t state) {
  uint8_t addr;
  if (get_addr_from_pin(pin, &addr)) return 1;

  uint8_t reg;
  if (get_gpio_from_pin(pin, &reg)) return 1;

  uint8_t port;
  if (twi_read_reg(addr, reg, &port)) return 1;


  uint8_t rel_pin = (pin % 16) % 8;
 
  port = (port & ~(1 << rel_pin)) | ((state & 0x01) << rel_pin);

  if (twi_write_reg(addr, reg, port)) return 1;

  return 0;
}

uint8_t mcp_write_pin_iodir(uint8_t pin, uint8_t state) {
  uint8_t addr;
  if (get_addr_from_pin(pin, &addr)) return 1;

  uint8_t reg;
  if (get_iodir_from_pin(pin, &reg)) return 1;

  uint8_t port;
  if (twi_read_reg(addr, reg, &port)) return 1;

  uint8_t rel_pin = (pin % 16) % 8;
 
  port = (port & ~(1 << rel_pin)) | ((state & 0x01) << rel_pin);

  if (twi_write_reg(addr, reg, port)) return 1;

  return 0;
}

uint8_t mcp_write_pin_pull_up(uint8_t pin, uint8_t state) {
  uint8_t addr;
  if (get_addr_from_pin(pin, &addr)) return 1;

  uint8_t reg;
  if (get_pull_up_from_pin(pin, &reg)) return 1;

  uint8_t port;
  if (twi_read_reg(addr, reg, &port)) return 1;

  uint8_t rel_pin = (pin % 16) % 8;
 
  port = (port & ~(1 << rel_pin)) | ((state & 0x01) << rel_pin);

  if (twi_write_reg(addr, reg, port)) return 1;

  return 0;
}

uint8_t mcp_read_pin(uint8_t pin, uint8_t* state) {
  uint8_t addr;
  if (get_addr_from_pin(pin, &addr)) return 1;

  uint8_t reg;
  if (get_gpio_from_pin(pin, &reg)) return 1;

  uint8_t port;
  if (twi_read_reg(addr, reg, &port)) return 1;

  uint8_t rel_pin = pin % 8;
 
  *state = (port >> rel_pin) & 1 ? pinl_HIGH : pinl_LOW;

  return 0;
}

uint8_t mcp_read_pin_olat(uint8_t pin, uint8_t* state) {
  uint8_t addr;
  if (get_addr_from_pin(pin, &addr)) return 1;

  uint8_t reg;
  if (get_olat_from_pin(pin, &reg)) return 1;

  uint8_t port;
  if (twi_read_reg(addr, reg, &port)) return 1;

  uint8_t rel_pin = pin % 8;

  *state = (port >> rel_pin) & 1 ? pinl_HIGH : pinl_LOW;

  return 0;
}

uint8_t mcp_read_pin_iodir(uint8_t pin, uint8_t* state) {
  uint8_t addr;
  if (get_addr_from_pin(pin, &addr)) return 1;

  uint8_t reg;
  if (get_iodir_from_pin(pin, &reg)) return 1;

  uint8_t port;
  if (twi_read_reg(addr, reg, &port)) return 1;

  uint8_t rel_pin = pin % 8;
  
  *state = (port >> rel_pin) & 1 ? pinm_INPUT : pinm_OUTPUT;

  return 0;
}

uint8_t mcp_read_pin_pull_up(uint8_t pin, uint8_t* state) {
  uint8_t addr;
  if (get_addr_from_pin(pin, &addr)) return 1;

  uint8_t reg;
  if (get_pull_up_from_pin(pin, &reg)) return 1;

  uint8_t port;
  if (twi_read_reg(addr, reg, &port)) return 1;

  uint8_t rel_pin = pin % 8;
 
  *state = (port >> rel_pin) & 1 ? pinl_HIGH : pinl_LOW;

  return 0;
}

