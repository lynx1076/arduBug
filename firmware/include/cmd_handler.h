#ifndef CMD_HANDLER_H
#define CMD_HANDLER_H

#include <stdint.h>

extern uint8_t cmd_response_buf[254];

uint8_t cmd_exec(uint8_t len, const uint8_t* cmd);

#endif
