#ifndef UTILS_H
#define UTILS_H

#include "stdlib.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum {
  log_INFO,
  log_OK,
  log_ERR,
  log_CRIT,
  log_WARN,
} log_level;

extern bool program_should_close;
extern bool dbg_log_to_user;

void debug_log(log_level ll, char* format, ...);
void sleep_us(size_t microseconds);
void sleep_ms(size_t milliseconds);
uint64_t micros(void);
uint64_t millis(void);
void print_hex(size_t len, const uint8_t* buf);
void panic(char* msg);

#endif
