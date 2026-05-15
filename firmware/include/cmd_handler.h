#ifndef CMD_HANDLER_H
#define CMD_HANDLER_H

#include "ucobs.h"
#include <stdint.h>

extern uint8_t cmd_response_buf[UCOBS_MAX_DATA_LEN];

uint8_t cmd_exec(uint8_t len, const uint8_t* cmd);

#endif
