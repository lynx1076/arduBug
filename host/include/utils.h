#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include <stdint.h>

typedef enum {
  log_INFO,
  log_OK,
  log_ERR,
  log_CRIT,
  log_WARN,
} log_level;

void debug_log(log_level ll, char* format, ...);
void sleep_us(size_t microseconds);
void sleep_ms(size_t milliseconds);
uint64_t micros(void);
uint64_t millis(void);

#endif
