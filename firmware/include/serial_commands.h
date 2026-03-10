#ifndef SERIAL_COMMANDS_H
#define SERIAL_COMMANDS_H

#include "serial_protocol.h"
#include <stdint.h>

#define SERIAL_COMMANDS_LIST \
  X(SP_CMD_PING, scmd_ping) \
  X(SP_CMD_WRITE, scmd_write) \
  X(SP_CMD_WRITE_MODE, scmd_write_mode) \
  X(SP_CMD_READ, scmd_read) \
  X(SP_CMD_VERSION_TXT, scmd_get_version_text) \
  X(SP_CMD_VERSION_CODE, scmd_get_version_code)

#define X(SP_COMMAND, FUNCTION) \
  uint8_t FUNCTION(uint8_t argc, uint8_t* argv);
SERIAL_COMMANDS_LIST
#undef X

#endif
