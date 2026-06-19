#ifndef UTILS_H
#define UTILS_H

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

extern bool dbg_log_to_user;

void debug_log(log_level ll, char* format, ...);
void quit(int exit_code);
void sleep_us(size_t microseconds);
void sleep_ms(size_t milliseconds);
uint64_t micros(void);
uint64_t millis(void);
int parse_long(const char* str, long* val);
int parse_hex_byte(const char* str, uint8_t* byte);
int parse_hex_word(const char* str, uint16_t* word);
void print_hex(size_t len, const uint8_t* buf);
void panic(char* msg);

#endif
