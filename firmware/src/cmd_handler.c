#include "cmd_handler.h"
#include "mcp23017.h"
#include "serial_protocol.h"
#include "meta.h"
#include <stdint.h>
#include <string.h>

uint8_t cmd_response_buf[254];

uint8_t cmd_exec(uint8_t len, const uint8_t* cmd) {
  const uint8_t* args = cmd + 1;
  uint8_t arg_cnt = len - 1;

  switch (*cmd) {
    case SP_CMD_PING: // Fall-through
    case SP_CMD_COMPAT_CODE: {
      *cmd_response_buf = SP_COMPAT_CODE;
      return 1;
    } break;
    case SP_CMD_VERSION_TEXT: {
      strncpy((char*)cmd_response_buf, VERSION_TXT, 254);
      return strlen(VERSION_TXT);
    } break;
    case SP_CMD_WRITE: {
      if (arg_cnt < 2) goto ERROR;
      if (mcp_write_pin(args[0], args[1])) goto ERROR;
    } // Fall-through
    case SP_CMD_READ: {
      if (arg_cnt < 1) goto ERROR;
      if (mcp_read_pin(args[0], cmd_response_buf)) goto ERROR;
      return 1;
    } break;
    case SP_CMD_WRITE_IODIR: {
      if (arg_cnt < 2) goto ERROR;
      if (mcp_write_pin_iodir(args[0], args[1])) goto ERROR;
    } // Fall-through
    case SP_CMD_READ_IODIR: {
      if (arg_cnt < 1) goto ERROR;
      if (mcp_read_pin_iodir(args[0], cmd_response_buf)) goto ERROR;
      return 1;
    } break;
    case SP_CMD_WRITE_PULL_UP: {
      if (arg_cnt < 2) goto ERROR;
      if (mcp_write_pin_pull_up(args[0], args[1])) goto ERROR;
    } // Fall-through
    case SP_CMD_READ_PULL_UP: {
      if (arg_cnt < 1) goto ERROR;
      if (mcp_read_pin_pull_up(args[0], cmd_response_buf)) goto ERROR;
      return 1;
    } break;
    default: goto ERROR;
  }

ERROR:
  *cmd_response_buf = SP_SIG_ERR;
  return 1;
}

