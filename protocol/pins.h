#ifndef N65_PINS_H
#define N65_PINS_H

#define MCP_IO0_PORTA              0x0A
#define MCP_IO0_PORTB              0x0B

#define MCP_IO1_PORTA              0x1A
#define MCP_IO1_PORTB              0x1B

#define N65_REG_DATABUS           MCP_IO0_PORTA
#define N65_REG_CTRL_PINS         MCP_IO0_PORTB

#define N65_REG_ADDR_LB           MCP_IO1_PORTA
#define N65_REG_ADDR_HB           MCP_IO1_PORTB

#define N65_CTRL_PIN_RWB          (1 << 0)
#define N65_CTRL_PIN_NMIB         (1 << 1)
#define N65_CTRL_PIN_IRQB         (1 << 2)
#define N65_CTRL_PIN_VPB          (1 << 3)
#define N65_CTRL_PIN_EXT_CLK_EN   (1 << 4)
#define N65_CTRL_PIN_EXT_CLK      (1 << 5)
#define N65_CTRL_PIN_EXT_RESETB   (1 << 6)
#define N65_CTRL_PIN_EXT_RWB      (1 << 7)

#endif
