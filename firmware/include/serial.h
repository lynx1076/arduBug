#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>

#define SER_BAUD_RATE     115200
#define SER_RX_BUF_SIZE   128

/*
 * Initialize serial with defined baudrate
*/
void ser_init();

/*
 * Get amount of available characters
*/
uint8_t ser_available(void);

/*
 * Read one byte from serial
 * Return 0 on success
*/
int ser_read(uint8_t* byte);

/*
 * Write byte to serial
*/
void ser_write(uint8_t byte);

/*
 * Write buffer to serial
*/
void ser_write_buf(uint8_t len, const uint8_t* buf);

/*
 * Write a null-terminated string to serial
*/
void ser_printf(const char* format, ...);

#endif
