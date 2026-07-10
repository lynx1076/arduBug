#include "cmd_handler.h"
#include "serial_protocol.h"
#include "meta.h"
#include "ucobs.h"
#include <stdint.h>
#include <string.h>

uint8_t cmd_response_buf[UCOBS_MAX_DATA_LEN];

uint8_t cmd_exec(uint8_t len, const uint8_t* cmd) {
  const uint8_t* args = cmd + 1;
  (void)args;
  uint8_t arg_cnt = len - 1;

  switch (*cmd) {
    case SP_CMD_PING: // Fall-through
    case SP_CMD_COMPAT_CODE: {
      *cmd_response_buf = SP_COMPAT_CODE;
      return 1;
    }
    case SP_CMD_VERSION_TEXT: {
      strncpy((char*)cmd_response_buf, VERSION_TXT, 254);
      return strlen(VERSION_TXT);
    }
    case SP_CMD_WRITE: {
      if (arg_cnt != 2) break;
      return 1;
    }
    case SP_CMD_READ: {
      if (arg_cnt != 1) break;
      return 1;
    }
    case SP_CMD_WRITE_IODIR: {
      if (arg_cnt != 2) break;
      return 1;
    }
    case SP_CMD_READ_IODIR: {
      if (arg_cnt != 1) break;
      return 1;
    }
    case SP_CMD_WRITE_PULL_UP: {
      if (arg_cnt != 2) break;
      return 1;
    }
    case SP_CMD_READ_PULL_UP: {
      if (arg_cnt != 1) break;
      return 1;
    }
    default: break;
  }

  *cmd_response_buf = SP_SIG_ERR;
  return 1;
}

