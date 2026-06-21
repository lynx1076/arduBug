#ifndef PINS_H
#define PINS_H

#include <stdint.h>

#define PINS_PORT_DATABUS           MCP_IO0_PORTA
#define PINS_PORT_CTRL              MCP_IO0_PORTB

#define PINS_PORT_ADDR_LB           MCP_IO1_PORTA
#define PINS_PORT_ADDR_HB           MCP_IO1_PORTB

typedef enum {
  pinl_UNKNOWN,
  pinl_HIGH,
  pinl_LOW
} PinState;

typedef enum {
  pinm_UNKNOWN,
  pinm_INPUT,
  pinm_OUTPUT
} PinMode;

typedef struct {
  uint8_t id;
  uint8_t ctx_id;
  char* name;

  PinMode mode;
  PinState state;

  uint8_t (*write)(uint8_t, PinState);
  uint8_t (*setpullup)(uint8_t, PinState);
  uint8_t (*setdir)(uint8_t, PinMode);
  uint8_t (*read)(uint8_t, PinState*);
} IOPin;

extern IOPin pins[];

#endif
