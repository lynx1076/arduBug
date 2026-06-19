#ifndef PINS_H
#define PINS_H

#define PIN_COUNT                   32

#define MCP_IO0_PORTA               0x0A
#define MCP_IO0_PORTB               0x0B

#define MCP_IO1_PORTA               0x1A
#define MCP_IO1_PORTB               0x1B

#define PINS_PORT_DATABUS           MCP_IO0_PORTA
#define PINS_PORT_CTRL              MCP_IO0_PORTB

#define PINS_PORT_ADDR_LB           MCP_IO1_PORTA
#define PINS_PORT_ADDR_HB           MCP_IO1_PORTB

#endif
