#ifndef SERIAL_H
#define SERIAL_H

#include "stdbool.h"
#include "result.h"
#include <stdint.h>

#define BAUD_RATE                     B115200
#define SERIAL_PORT_PATH_MAX          300
#define MAX_PORTS                     128

typedef struct {
  char port_name[SERIAL_PORT_PATH_MAX];
  char full_path[SERIAL_PORT_PATH_MAX];
} ser_SerialPort;

/*
 * Initalize internal variables and buffers
*/
result ser_init(void);

/*
 * Free internal variables and buffers
 * Closes active serial connection
*/
void ser_free(void);

/*
 * Update internal variables and states
*/
void ser_update(void);

/*
 * Return amount of available ports and save them in internal buffer
 * Return -1 on failure
*/
result ser_scan_ports(int* count);

/*
 * Return name of port at index from internal buffer
 * Return NULL on failure
*/
char* ser_get_port_name(int index);

/*
 * Open serial port based on it's index in internal buffer
 * Return -1 on failure
*/
result ser_open_by_id(int id);

/*
 * Close open port
 * Does nothing if no port open
*/
void ser_close(void);

/*
 * Return pointer to name of open port
 * Return NULL on failure or if no port is open
*/
char* ser_get_open(void);

/*
 * Return true when port is open
*/
bool ser_is_open(void);

/*
 * Return true if port just closed
 * Return false on every subsequent call
*/
bool ser_just_closed(void);

/*
 * Return true if port just opened
 * Return false on every subsequent call
*/
bool ser_just_opened(void);

/*
 * Write a single byte to open serial port
 * Return -1 on failure and close port
*/
result ser_write(uint8_t data);

/*
 * Read a single byte from open serial port
 * Return -1 on failure and close port
*/
result ser_read(uint8_t* data);

/*
 * Flush input and output of serial port
*/
result ser_flush();

#endif
