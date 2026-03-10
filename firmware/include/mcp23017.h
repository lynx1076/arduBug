#ifndef MCP23017_H
#define MCP23017_H

#include <stdint.h>
#include <stdbool.h>

uint8_t mcp_check_health(void);
uint8_t mcp_write_port(uint8_t port, uint8_t gpio);
uint8_t mcp_write_port_mode(uint8_t port, uint8_t iodir, uint8_t pull_up);
uint8_t mcp_read_port(uint8_t port, uint8_t* gpio);

#endif
