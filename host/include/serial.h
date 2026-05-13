#ifndef SERIAL_H
#define SERIAL_H

#include "stdbool.h"
#include "result.h"
#include "vector.h"
#include <stdint.h>

#define SER_BAUDRATE                  B115200
#define SERIAL_PORT_PATH_MAX          300

#define READ_TIMEOUT_100MS            2

typedef enum {
  portState_disconnected,
  portState_connecting,
  portState_connected
} e_PortState;

result ser_update(void);
result ser_scan_ports(Vec* return_vec);
result ser_open(char* path);
void ser_close(void);
char* ser_get_current_port(void);
bool ser_is_open(void);
bool ser_just_opened(void);
bool ser_just_closed(void);
result ser_write(size_t length, const uint8_t* data);
result ser_enc_write_va(size_t length, ...);
result ser_read(size_t* length, uint8_t* data);
result ser_enc_read(int* length, uint8_t* data);

#endif
