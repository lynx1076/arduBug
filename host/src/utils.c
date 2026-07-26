#include "utils.h"
#include "gui.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <strings.h>

bool program_should_close = false;
bool dbg_log_to_user = false;

static char* debug_log_get_prefix(log_level ll) {
  switch (ll) {
  case log_INFO:
    return "INFO";
  case log_ERR:
    return "ERR";
  case log_CRIT:
    return "CRIT";
  case log_WARN:
    return "WARN";
  case log_OK:
    return "OK";
  default:
    return "UNKNOWN";
  }
}

void debug_log(log_level ll, char* format, ...) {
  va_list va_args;

  va_start(va_args, format);

  char* prefix = debug_log_get_prefix(ll);
  char* msg;
  vasprintf(&msg, format, va_args);

  char* full_str;
  asprintf(&full_str, "[%s] %s", prefix, msg);

  if (dbg_log_to_user) gui_log(full_str);
  printf("%s\n", full_str);

  free(full_str);
  
  return;
}

void sleep_us(size_t microseconds) {
  struct timespec delay;

  delay.tv_sec = microseconds / 1000000;
  delay.tv_nsec = (microseconds % 1000000) * 1000;

  nanosleep(&delay, NULL);
}

void sleep_ms(size_t milliseconds) {
  struct timespec delay;

  delay.tv_sec = milliseconds / 1000;
  delay.tv_nsec = (milliseconds % 1000) * 1000000;

  nanosleep(&delay, NULL);
}

uint64_t micros(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);

  return (uint64_t)ts.tv_sec * 1000000ULL + ts.tv_nsec / 1000;
}

uint64_t millis(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);

  return (uint64_t)ts.tv_sec * 1000ULL + ts.tv_nsec / 1000000ULL;
}

void print_hex(size_t len, const uint8_t* buf) {
  for (size_t i = 0; i < len; i++) {
    printf("0x%02x ", buf[i]);
  }
  printf("\n");
}

void panic(char* msg) {
  printf("\n\n");
  printf("WARNING: PROGRAM PANIC - EXITING IMMEDIATELY\n");
  printf("%s", msg);
}

