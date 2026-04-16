#ifndef MCP23017_H
#define MCP23017_H

#include <stdint.h>
#include <stdbool.h>

uint8_t mcp_write_pin(uint8_t pin, uint8_t state);
uint8_t mcp_write_pin_iodir(uint8_t pin, uint8_t state);
uint8_t mcp_write_pin_pull_up(uint8_t pin, uint8_t state);

uint8_t mcp_read_pin(uint8_t pin, uint8_t* state);
uint8_t mcp_read_pin_iodir(uint8_t pin, uint8_t* state);
uint8_t mcp_read_pin_pull_up(uint8_t pin, uint8_t* state);

void test(void);

#endif
