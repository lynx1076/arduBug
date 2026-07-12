#ifndef CMD_HANDLER_H
#define CMD_HANDLER_H

#include <stdint.h>

uint8_t cmd_exec(uint8_t len, const uint8_t* cmd, uint8_t* resp);

#endif
