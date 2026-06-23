#ifndef IO_H
#define IO_H

#define PINS_PORT_DATABUS           MCP_IO0_PORTA
#define PINS_PORT_CTRL              MCP_IO0_PORTB

#define PINS_PORT_ADDR_LB           MCP_IO1_PORTA
#define PINS_PORT_ADDR_HB           MCP_IO1_PORTB

// Signal numbers according to connector-index in schematic

#define IO_EXT_CPU_EN           1
#define IO_EXT_RESETB           2
#define 

#define IO_DATA_0               3
#define IO_DATA_1               5
#define IO_DATA_2               7
#define IO_DATA_3               9
#define IO_DATA_4               13
#define IO_DATA_5               15
#define IO_DATA_6               17
#define IO_DATA_7               19


typedef enum {
  io_UNKNOWN,
  io_HIGH,
  io_LOW
} IOState;

typedef enum {
  iom_UNKNOWN,
  iom_INPUT,
  iom_OUTPUT
} IOMode;

#endif
