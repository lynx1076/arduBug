#ifndef MCP23017_H
#define MCP23017_H

#include <stdint.h>
#include <stdbool.h>

#define MCP_IO0_PORTA               0x0A
#define MCP_IO0_PORTB               0x0B

#define MCP_IO1_PORTA               0x1A
#define MCP_IO1_PORTB               0x1B

uint8_t mcp_write_pin(uint8_t pin, uint8_t state);
uint8_t mcp_write_pin_iodir(uint8_t pin, uint8_t state);
uint8_t mcp_write_pin_pull_up(uint8_t pin, uint8_t state);

uint8_t mcp_read_pin(uint8_t pin, uint8_t* state);
uint8_t mcp_read_pin_olat(uint8_t pin, uint8_t* state);
uint8_t mcp_read_pin_iodir(uint8_t pin, uint8_t* state);
uint8_t mcp_read_pin_pull_up(uint8_t pin, uint8_t* state);

#endif

