#ifndef SERIAL_HANDLER_H
#define SERIAL_HANDLER_H

#include <stdint.h>

void seh_receive(void (*onRead)(uint8_t, uint8_t*));
void seh_transmit_buf(uint8_t len, const uint8_t* data);

#endif
