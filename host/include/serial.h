#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define SER_BAUD_RATE                 B9600
#define SER_MAX_PORT_PATH_LENGTH      256


/**
 * Initalize serial functions and port vector
 * Returns 0 on success, -1 on failure
 */
int ser_init(void);

/**
 * Deinitalize serial functions and port vector
 */
void ser_free(void);

/**
 * Open and configure a serial port in non-blocking mode
 * Returns 0 on success, -1 on failure
 */
int ser_open(const char* port);

/**
 * Close the serial port
 */
void ser_close(void);

/**
 * Return the number of bytes currently readable (internal RX buffer + driver queue)
 * Returns -1 if the port is not open
 */
int ser_available(void);

/**
 * Read up to len bytes into buf
 * Returns the number of bytes read (0 if no data is available), or -1 on error
 */
int ser_read(uint8_t *buf, size_t len);

/**
 * Write len bytes from buf to the serial port
 * Returns 0 if all bytes were written, -1 on error
 */
int ser_write(const uint8_t *buf, size_t len);

/**
 * Scan all open ports into the port vector
 * Return available port count on success, -1 on fialure
 */
int ser_scan_ports(void);

/**
 * Return pointer to null terminated string of the port name at index
 * Return null on failure
 */
char* ser_get_port_name(int index);

#endif
