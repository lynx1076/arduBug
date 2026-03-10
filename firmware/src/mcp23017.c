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

#define MCP_PULLUPA       0x0C
#define MCP_PULLUPB       0x0D

#define MCP_PORT_VALID(PORT) \
  ( \
    (PORT) == MCP_IO0_PORTA || (PORT) == MCP_IO0_PORTB || \
    (PORT) == MCP_IO1_PORTA || (PORT) == MCP_IO1_PORTB \
  )

#define MCP_ADDR(PORT) \
  ( \
    ((PORT) == MCP_IO0_PORTA || (PORT) == MCP_IO0_PORTB) \
    ? MCP_ADDR_IO0 : MCP_ADDR_IO1 \
  )

#define MCP_GPIO(PORT) \
  ( \
    ((PORT) == MCP_IO0_PORTA || (PORT) == MCP_IO1_PORTA) \
    ? MCP_GPIOA : MCP_GPIOB \
  )

#define MCP_IODIR(PORT) \
  ( \
    ((PORT) == MCP_IO0_PORTA || (PORT) == MCP_IO1_PORTA) \
    ? MCP_IODIRA : MCP_IODIRB \
  )

#define MCP_PULLUP(PORT) \
  ( \
    ((PORT) == MCP_IO0_PORTA || (PORT) == MCP_IO1_PORTA) \
    ? MCP_PULLUPA : MCP_PULLUPB \
  )

uint8_t mcp_check_health(void) {
  uint8_t status = 0;

  status |= twi_check_device_present(MCP_ADDR_IO0);
  status |= twi_check_device_present(MCP_ADDR_IO1);

  return status;
}

uint8_t mcp_write_port(uint8_t port, uint8_t gpio) {
  if (!MCP_PORT_VALID(port)) return 1;

  if (twi_write_reg(MCP_ADDR(port), MCP_GPIO(port), gpio)) return 1;

  return 0;
}

uint8_t mcp_write_port_mode(uint8_t port, uint8_t iodir, uint8_t pull_up) {
  if (!MCP_PORT_VALID(port)) return 1;

  if (twi_write_reg(MCP_ADDR(port), MCP_IODIR(port), iodir)) return 1;
  if (twi_write_reg(MCP_ADDR(port), MCP_PULLUP(port), pull_up)) return 1;

  return 0;
}

uint8_t mcp_read_port(uint8_t port, uint8_t* gpio) {
  if (!MCP_PORT_VALID(port)) return 1;

  return twi_read_reg(MCP_ADDR(port), MCP_GPIO(port), gpio);
}

