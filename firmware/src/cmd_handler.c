#include "cmd_handler.h"
#include "serial_protocol.h"
#include "meta.h"
#include <stdint.h>
#include <string.h>

uint8_t cmd_exec(uint8_t len, const uint8_t* cmd, uint8_t* resp) {
  const uint8_t* args = cmd + 1;
  uint8_t arg_cnt = len - 1;
  (void)args;
  (void)arg_cnt;

  switch (*cmd) {
    case SP_CMD_PING:
    case SP_CMD_COMPAT_CODE: {
      *resp = SP_COMPAT_CODE;
      return 1;
    }
    case SP_CMD_VERSION_TEXT: {
      strncpy((char*)resp, VERSION_TXT, 254);
      return strlen(VERSION_TXT);
    }
    default: break;
  }

  *resp = SP_SIG_ERR;
  return 1;
}

