#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>

#define SER_RX_BUF_SIZE       64
#define SER_BAUD_RATE     115200

void ser_init();
uint8_t ser_available(void);

uint8_t ser_read(uint8_t* byte);
void ser_write(uint8_t byte);

#endif
