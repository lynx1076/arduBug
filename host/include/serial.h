#ifndef SERIAL_H
#define SERIAL_H

#include "stdbool.h"
#include "vector.h"
#include <stddef.h>
#include <stdint.h>

#define SER_BAUDRATE                  B115200
#define SERIAL_PORT_PATH_MAX          300

int ser_update(void);
int ser_scan_ports(Vec* return_vec);
int ser_open(char* path);
void ser_close(void);
char* ser_get_device(void);
bool ser_is_open(void);
bool ser_just_opened(void);
bool ser_just_closed(void);
int ser_write(size_t length, const uint8_t* data);
int ser_read(size_t* length, uint8_t* data);

int ser_enc_read(size_t* length, uint8_t* data);

int ser_enc_write_va(size_t length, ...);
int ser_enc_read_va(size_t expected_length, ...);

#endif
