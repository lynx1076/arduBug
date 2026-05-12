#include "utils.h"
#include "result.h"
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

bool dbg_log_to_printf = false;

void (*LOG_FUNCS[])(const char*) = {
  NULL
};

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
    return "[ll_unknown]";
  }
}

void debug_log(log_level ll, char* format, ...) {
  va_list va_args;

  va_start(va_args, format);

  char* prefix = debug_log_get_prefix(ll);
  char* msg;
  vasprintf(&msg, format, va_args);
  if (msg == NULL) goto ERROR;

  char* full_str;
  asprintf(&full_str, "[%s] %s", prefix, msg);

  if (dbg_log_to_printf) printf("%s\n", full_str);
  for (int i = 0; LOG_FUNCS[i] != 0; i++) {
    LOG_FUNCS[i](full_str);
  }

  return;

ERROR:
  printf("\n");
  printf("LOGGING FAILED!\n");
  printf("Certain errors and events might have not been properly logged!\n");
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

result parse_int(const char* str, long* val) {
  errno = 0;
  char* endptr;
  *val = strtol(str, &endptr, 10);

  if (errno != 0) return r_EUSER_PARSE;

  return r_ENONE;
}

