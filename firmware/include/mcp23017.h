#ifndef MCP23017_H
#define MCP23017_H

#include "io.h"
#include <stdint.h>
#include <stdbool.h>

#define MCP_0_PA0 0
#define MCP_0_PA1 1
#define MCP_0_PA2 2
#define MCP_0_PA3 3
#define MCP_0_PA4 4
#define MCP_0_PA5 5
#define MCP_0_PA6 6
#define MCP_0_PA7 7

#define MCP_0_PB0 8
#define MCP_0_PB1 9
#define MCP_0_PB2 10
#define MCP_0_PB3 11
#define MCP_0_PB4 12
#define MCP_0_PB5 13
#define MCP_0_PB6 14
#define MCP_0_PB7 15

#define MCP_1_PA0 16
#define MCP_1_PA1 17
#define MCP_1_PA2 18
#define MCP_1_PA3 19
#define MCP_1_PA4 20
#define MCP_1_PA5 21
#define MCP_1_PA6 22
#define MCP_1_PA7 23

#define MCP_1_PB0 24
#define MCP_1_PB1 25
#define MCP_1_PB2 26
#define MCP_1_PB3 27
#define MCP_1_PB4 28
#define MCP_1_PB5 29
#define MCP_1_PB6 30
#define MCP_1_PB7 31

typedef struct {
  uint8_t pin;
} MCPContext;

extern const IOVtable mcp_vtable;


uint8_t mcp_write_pin_iodir(MCPContext* ctx, IOMode mode);
uint8_t mcp_write_pin(MCPContext* ctx, IOState state);
uint8_t mcp_read_pin(MCPContext* ctx, IOState* state);
uint8_t mcp_read_olat(MCPContext* ctx, IOState* state);
uint8_t mcp_highz(MCPContext* ctx);

uint8_t mcp_write_pin_iodir_w(IOContext* ctx, IOMode mode);
uint8_t mcp_write_pin_w(IOContext* ctx, IOState state);
uint8_t mcp_read_pin_w(IOContext* ctx, IOState* state);
uint8_t mcp_read_olat_w(IOContext* ctx, IOState* state);
uint8_t mcp_highz_w(IOContext* ctx);

#endif

