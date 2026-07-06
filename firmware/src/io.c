#include "io.h"
#include "mcp23017.h"
#include "pins.h"
#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <stdint.h>


uint8_t io_write(IOPin* pin, IOMode mode, IOState state) {
  switch (pin->id) {
    case PinId_DATA_0:
    case PinId_DATA_1:
    case PinId_DATA_2:
    case PinId_DATA_3: {
      if (pin->mode != mode) {
        DDRD = (DDRD & ~_BV(pin->ctx_id)) | (mode == iom_INPUT ? _BV(pin->ctx_id) : 0);
        pin->mode = mode;
        pin->state = io_UNKNOWN;
      }

      if (pin->state != state) {
        PORTD = (PORTD & ~_BV(pin->ctx_id)) | (state == io_HIGH ? _BV(pin->ctx_id) : 0);
        pin->state = state;
      }
    } break;
    case PinId_DATA_4:
    case PinId_DATA_5:
    case PinId_DATA_6:
    case PinId_DATA_7: {
      if (pin->mode != mode) {
        DDRB = (DDRB & ~_BV(pin->ctx_id)) | (mode == iom_INPUT ? _BV(pin->ctx_id) : 0);
        pin->mode = mode;
        pin->state = io_UNKNOWN;
      }

      if (pin->state != state) {
        PORTB = (PORTB & ~_BV(pin->ctx_id)) | (state == io_HIGH ? _BV(pin->ctx_id) : 0);
        pin->state = state;
      }
    } break;
    case PinId_ADDR_00:
    case PinId_ADDR_01:
    case PinId_ADDR_02:
    case PinId_ADDR_03:
    case PinId_ADDR_04:
    case PinId_ADDR_05:
    case PinId_ADDR_06:
    case PinId_ADDR_07:
    case PinId_ADDR_08:
    case PinId_ADDR_09:
    case PinId_ADDR_10:
    case PinId_ADDR_11:
    case PinId_ADDR_12:
    case PinId_ADDR_13:
    case PinId_ADDR_14:
    case PinId_ADDR_15: {
      if (pin->mode != mode) {
        if (mcp_write_pin_iodir(pin->ctx_id, mode)) return -1;
        pin->mode = mode;
        pin->state = io_UNKNOWN;
      }

      if (pin->state != state) {
        if (mcp_write_pin(pin->ctx_id, state))
        pin->state = state;
      }
    } break;
  }

  return 0;
}

uint8_t io_read(IOPin* pin, IOMode* mode, IOState* state) {


  return 0;
}

