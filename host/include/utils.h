#ifndef UTILS_H
#define UTILS_H

#include "result.h"
#include "stdlib.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define BOOL_CMP(A, B)          (!((A)^(B)))

typedef enum {
  log_INFO,
  log_OK,
  log_ERR,
  log_CRIT,
  log_WARN,
} log_level;

extern bool dbg_log_to_stdout;

void debug_log(log_level ll, char* format, ...);
void sleep_us(size_t microseconds);
void sleep_ms(size_t milliseconds);
uint64_t micros(void);
uint64_t millis(void);
result parse_long(const char* str, long* val);
result parse_hex_byte(const char* str, uint8_t* byte);
void print_hex(size_t len, const uint8_t* buf);

#endif
