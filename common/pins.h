#ifndef PINS_H
#define PINS_H

#define LOW                         0
#define HIGH                        1

#define INPUT                       HIGH
#define OUTPUT                      LOW

#define PIN_COUNT                   32

#define MCP_IO0_PORTA               0x0A
#define MCP_IO0_PORTB               0x0B

#define MCP_IO1_PORTA               0x1A
#define MCP_IO1_PORTB               0x1B

#define PINS_PORT_DATABUS           MCP_IO0_PORTA
#define PINS_PORT_CTRL              MCP_IO0_PORTB

#define PINS_PORT_ADDR_LB           MCP_IO1_PORTA
#define PINS_PORT_ADDR_HB           MCP_IO1_PORTB

#define PINS_CTRL_PIN_RWB           8
#define PINS_CTRL_PIN_NMIB          9
#define PINS_CTRL_PIN_IRQB          10
#define PINS_CTRL_PIN_VPB           11
#define PINS_CTRL_PIN_EXT_CLK_EN    12
#define PINS_CTRL_PIN_EXT_CLK       13
#define PINS_CTRL_PIN_EXT_RESETB    14
#define PINS_CTRL_PIN_EXT_RWB       15

#define PINS_DATA_PIN_0             0
#define PINS_DATA_PIN_1             1
#define PINS_DATA_PIN_2             2
#define PINS_DATA_PIN_3             3
#define PINS_DATA_PIN_4             4
#define PINS_DATA_PIN_5             5
#define PINS_DATA_PIN_6             6
#define PINS_DATA_PIN_7             7

#define PINS_ADDR_PIN_0             16
#define PINS_ADDR_PIN_1             17
#define PINS_ADDR_PIN_2             18
#define PINS_ADDR_PIN_3             19
#define PINS_ADDR_PIN_4             20
#define PINS_ADDR_PIN_5             21
#define PINS_ADDR_PIN_6             22
#define PINS_ADDR_PIN_7             23

#define PINS_ADDR_PIN_8             24
#define PINS_ADDR_PIN_9             25
#define PINS_ADDR_PIN_10            26
#define PINS_ADDR_PIN_11            27
#define PINS_ADDR_PIN_12            28
#define PINS_ADDR_PIN_13            29
#define PINS_ADDR_PIN_14            30
#define PINS_ADDR_PIN_15            31

#endif
