#ifndef MCP23017_H
#define MCP23017_H

#include "io.h"
#include <stdint.h>
#include <stdbool.h>

#define MCP_IO0_PORTA               0x0A
#define MCP_IO0_PORTB               0x0B

#define MCP_IO1_PORTA               0x1A
#define MCP_IO1_PORTB               0x1B

#define MCP_HIGH        1
#define MCP_LOW         0
#define MCP_OUTPUT      0
#define MCP_INPUT       1

uint8_t mcp_write_pin(uint8_t pin, IOState state);
uint8_t mcp_write_pin_iodir(uint8_t pin, IOMode mode);

uint8_t mcp_read_pin(uint8_t pin, IOState* state);

#endif

