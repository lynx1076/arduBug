#ifndef PINS_H
#define PINS_H

#define LOW                         0
#define HIGH                        1

#define INPUT                       1
#define OUTPUT                      0

#define MCP_IO0_PORTA               0x0A
#define MCP_IO0_PORTB               0x0B

#define MCP_IO1_PORTA               0x1A
#define MCP_IO1_PORTB               0x1B

#define PINS_PORT_DATABUS           MCP_IO0_PORTA
#define PINS_PORT_CTRL              MCP_IO0_PORTB

#define PINS_PORT_ADDR_LB           MCP_IO1_PORTA
#define PINS_PORT_ADDR_HB           MCP_IO1_PORTB

#define PINS_CTRL_PIN_RWB           0
#define PINS_CTRL_PIN_NMIB          1
#define PINS_CTRL_PIN_IRQB          2
#define PINS_CTRL_PIN_VPB           3
#define PINS_CTRL_PIN_EXT_CLK_EN    4
#define PINS_CTRL_PIN_EXT_CLK       5
#define PINS_CTRL_PIN_EXT_RESETB    6
#define PINS_CTRL_PIN_EXT_RWB       7

#endif
