#include "io_defs.h"
#include "mcp23017.h"
#include "mcu_io.h"


IOPin pin_EXT_CPU_EN = { .name = "EXT_CPU_EN", .id = 1, .vtable = &mcp_vtable, .context = &(MCPContext){ .pin = MCP_1_PA2 } };
IOPin pin_EXT_RESETB = { .name = "EXT_RESETB", .id = 2, .vtable = &mcp_vtable, .context = &(MCPContext){ .pin = MCP_1_PA0 } };
IOPin pin_EXT_CLK_EN = { .name = "EXT_CLK_EN", .id = 4, .vtable = &mcp_vtable, .context = &(MCPContext){ .pin = MCP_1_PA1 } };
IOPin pin_EXT_CLK = { .name = "EXT_CLK", .id = 31, .vtable = &mcp_vtable, .context = &(MCPContext){ .pin = MCP_1_PA3 } };
IOPin pin_DEV_TBO = { .name = "DEV_TBO", .id = 21, .vtable = &mcp_vtable, .context = &(MCPContext){ .pin = MCP_0_PA4 } };
IOPin pin_VPB = { .name = "VPB", .id = 23, .vtable = &mcp_vtable, .context = &(MCPContext){ .pin = MCP_1_PA4 } };
IOPin pin_SYNC = { .name = "SYNC", .id = 25, .vtable = &mcp_vtable, .context = &(MCPContext){ .pin = MCP_1_PA5 } };

IOPin pin_DEV_ENB = { .name = "DEV_ENB", .id = 33, .vtable = &mcp_vtable, .context = &(MCPContext){ .pin = MCP_1_PB0 } };
IOPin pin_RWB = { .name = "RWB", .id = 35, .vtable = &mcp_vtable, .context = &(MCPContext){ .pin = MCP_1_PB1 } };
IOPin pin_NMIB = { .name = "NMIB", .id = 37, .vtable = &mcp_vtable, .context = &(MCPContext){ .pin = MCP_1_PB2 } };
IOPin pin_IRQB = { .name = "IRQB", .id = 39, .vtable = &mcp_vtable, .context = &(MCPContext){ .pin = MCP_1_PB3 } };

IOPin pin_DATA_0 = { .name = "DATA_0", .id = 3, .vtable = &mio_vtable, .context = &(MIOContext){ .pin = MIO_PD4 } };
IOPin pin_DATA_1 = { .name = "DATA_1", .id = 5, .vtable = &mio_vtable, .context = &(MIOContext){ .pin = MIO_PD5 } };
IOPin pin_DATA_2 = { .name = "DATA_2", .id = 7, .vtable = &mio_vtable, .context = &(MIOContext){ .pin = MIO_PD6 } };
IOPin pin_DATA_3 = { .name = "DATA_3", .id = 9, .vtable = &mio_vtable, .context = &(MIOContext){ .pin = MIO_PD7 } };
IOPin pin_DATA_4 = { .name = "DATA_4", .id = 13, .vtable = &mio_vtable, .context = &(MIOContext){ .pin = MIO_PB0 } };
IOPin pin_DATA_5 = { .name = "DATA_5", .id = 15, .vtable = &mio_vtable, .context = &(MIOContext){ .pin = MIO_PB1 } };
IOPin pin_DATA_6 = { .name = "DATA_6", .id = 17, .vtable = &mio_vtable, .context = &(MIOContext){ .pin = MIO_PB2 } };
IOPin pin_DATA_7 = { .name = "DATA_7", .id = 19, .vtable = &mio_vtable, .context = &(MIOContext){ .pin = MIO_PB3 } };

IOPin pin_ADDR_00 = { .name = "ADDR_00", .id = 6,  .vtable = &mcp_vtable, .context = &(MCPContext){ .pin = MCP_0_PA0 } };
IOPin pin_ADDR_01 = { .name = "ADDR_01", .id = 8,  .vtable = &mcp_vtable, .context = &(MCPContext){ .pin = MCP_0_PA1 } };
IOPin pin_ADDR_02 = { .name = "ADDR_02", .id = 10, .vtable = &mcp_vtable, .context = &(MCPContext){ .pin = MCP_0_PA2 } };
IOPin pin_ADDR_03 = { .name = "ADDR_03", .id = 12, .vtable = &mcp_vtable, .context = &(MCPContext){ .pin = MCP_0_PA3 } };
IOPin pin_ADDR_04 = { .name = "ADDR_04", .id = 14, .vtable = &mcp_vtable, .context = &(MCPContext){ .pin = MCP_0_PA4 } };
IOPin pin_ADDR_05 = { .name = "ADDR_05", .id = 16, .vtable = &mcp_vtable, .context = &(MCPContext){ .pin = MCP_0_PA5 } };
IOPin pin_ADDR_06 = { .name = "ADDR_06", .id = 18, .vtable = &mcp_vtable, .context = &(MCPContext){ .pin = MCP_0_PA6 } };
IOPin pin_ADDR_07 = { .name = "ADDR_07", .id = 20, .vtable = &mcp_vtable, .context = &(MCPContext){ .pin = MCP_0_PA7 } };
IOPin pin_ADDR_08 = { .name = "ADDR_08", .id = 22, .vtable = &mcp_vtable, .context = &(MCPContext){ .pin = MCP_0_PB0 } };
IOPin pin_ADDR_09 = { .name = "ADDR_09", .id = 24, .vtable = &mcp_vtable, .context = &(MCPContext){ .pin = MCP_0_PB1 } };
IOPin pin_ADDR_10 = { .name = "ADDR_10", .id = 26, .vtable = &mcp_vtable, .context = &(MCPContext){ .pin = MCP_0_PB2 } };
IOPin pin_ADDR_11 = { .name = "ADDR_11", .id = 28, .vtable = &mcp_vtable, .context = &(MCPContext){ .pin = MCP_0_PB3 } };
IOPin pin_ADDR_12 = { .name = "ADDR_12", .id = 30, .vtable = &mcp_vtable, .context = &(MCPContext){ .pin = MCP_0_PB4 } };
IOPin pin_ADDR_13 = { .name = "ADDR_13", .id = 32, .vtable = &mcp_vtable, .context = &(MCPContext){ .pin = MCP_0_PB5 } };
IOPin pin_ADDR_14 = { .name = "ADDR_14", .id = 34, .vtable = &mcp_vtable, .context = &(MCPContext){ .pin = MCP_0_PB6 } };
IOPin pin_ADDR_15 = { .name = "ADDR_15", .id = 36, .vtable = &mcp_vtable, .context = &(MCPContext){ .pin = MCP_0_PB7 } };

