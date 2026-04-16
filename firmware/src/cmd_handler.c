#include "cmd_handler.h"
#include "mcp23017.h"
#include "serial_protocol.h"
#include "serial_handler.h"
#include "meta.h"
#include "utils.h"
#include <stdint.h>

void cmd_exec(uint8_t len, uint8_t* cmd) {
  uint8_t* args = cmd + 1;
  uint8_t arg_cnt = len - 1;

  switch (*cmd) {
    case SP_CMD_RESET: {
      seh_transmit_buf(1, (uint8_t[]){0x00});
      reset();
    } break;
    case SP_CMD_PING: // Fall-through
    case SP_CMD_COMPAT_CODE: {
      seh_transmit_buf(1, (uint8_t[]){SP_COMPAT_CODE});
    } break;
    case SP_CMD_VERSION_TEXT: {
      seh_transmit_buf(sizeof(VERSION_TXT) / sizeof(*VERSION_TXT) - 1, (uint8_t*)VERSION_TXT);
    } break;
    case SP_CMD_WRITE: {
      if (arg_cnt < 2) goto ERROR;
      if (mcp_write_pin(args[0], args[1])) goto ERROR;
    } // Fall-through
    case SP_CMD_READ: {
      if (arg_cnt < 1) goto ERROR;
      uint8_t result;
      if (mcp_read_pin(args[0], &result)) goto ERROR;
      seh_transmit_buf(1, &result);
    } break;
    case SP_CMD_WRITE_IODIR: {
      if (arg_cnt < 2) goto ERROR;
      if (mcp_write_pin_iodir(args[0], args[1])) goto ERROR;
    } // Fall-through
    case SP_CMD_READ_IODIR: {
      if (arg_cnt < 1) goto ERROR;
      uint8_t result;
      if (mcp_read_pin_iodir(args[0], &result)) goto ERROR;
      seh_transmit_buf(1, &result);
    } break;
    case SP_CMD_WRITE_PULL_UP: {
      if (arg_cnt < 2) goto ERROR;
      if (mcp_write_pin_pull_up(args[0], args[1])) goto ERROR;
    } // Fall-through
    case SP_CMD_READ_PULL_UP: {
      if (arg_cnt < 1) goto ERROR;
      uint8_t result;
      if (mcp_read_pin_pull_up(args[0], &result)) goto ERROR;
      seh_transmit_buf(1, &result);
    } break;
    default: goto ERROR;
  }
  
  return;

ERROR:
  seh_transmit_buf(1, (uint8_t[]){SP_SIG_ERR});
}

