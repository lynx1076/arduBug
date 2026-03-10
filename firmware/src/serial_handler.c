#include "serial_handler.h"
#include "serial_protocol.h"
#include "serial_commands.h"
#include "serial.h"
#include <stdint.h>
#include <stdbool.h>

typedef enum {
  SEH_STATE_IDLE,
  SEH_STATE_CMD,
  SEH_STATE_ARGS
} seh_state;

static seh_state state = SEH_STATE_IDLE;
static bool esc = false;

static uint8_t cmd = 0;
static uint8_t arg_buff[SEH_MAX_ARG_CNT];
static uint8_t arg_cnt = 0;


static uint8_t seh_execute_command(void) {
#define X(SP_COMMAND, FUNCTION) \
  if (cmd == SP_COMMAND) { \
    return FUNCTION(arg_cnt, arg_buff); \
  }
  SERIAL_COMMANDS_LIST
#undef X
  return 1;
}

void seh_handle_command(void) {
  int16_t read = ser_readc();
  if (read < 0) {
    return;
  }

  uint8_t data = (uint8_t)read;

  if (data == SP_SIG_ESC) {
    if (!esc) {
      esc = true;
      return;
    }
  }

  switch (state) {
    case SEH_STATE_IDLE:
      if (data == SP_SIG_SYNC && !esc) state = SEH_STATE_CMD;
      break;
    case SEH_STATE_CMD:
      if (data == SP_SIG_SYNC && !esc) {
        break;
      }
      cmd = data;
      arg_cnt = 0;
      state = SEH_STATE_ARGS;
      break;
    case SEH_STATE_ARGS:
      if (data == SP_SIG_SYNC && !esc) {
        state = SEH_STATE_IDLE;
        ser_write_raw(SP_SIG_SYNC);
        uint8_t result = seh_execute_command();
        ser_write_raw(result ? SP_SIG_NACK : SP_SIG_ACK);
      } else if (arg_cnt >= SEH_MAX_ARG_CNT) {
        state = SEH_STATE_IDLE;
        ser_write_raw(SP_SIG_SYNC);
        ser_write_raw(SP_SIG_NACK);
      } else {
        arg_buff[arg_cnt++] = data;
      }
      break;
  }
  
  esc = false;
}

