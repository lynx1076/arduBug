#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>

#define SER_RX_BUFF_SIZE          64

void ser_init(uint32_t baudrate);
uint8_t ser_available(void);

int16_t ser_readc(void);
uint8_t ser_read(uint8_t* buf, uint8_t buf_size);

void ser_write_raw(uint8_t byte);
void ser_write(uint8_t c);

void ser_write_string(const char* str, uint8_t len);

#endif
