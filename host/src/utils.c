#include "utils.h"
#include "result.h"
#include <bits/pthreadtypes.h>
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <time.h>
#include <ncurses.h>

bool dbg_log_to_user = false;

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

  if (dbg_log_to_user) printw("%s\n", full_str);
  for (int i = 0; LOG_FUNCS[i] != 0; i++) {
    LOG_FUNCS[i](full_str);
  }

  return;

ERROR:
  printf("\n");
  printf("LOGGING FAILED!\n");
  printf("Certain errors and events might have not been properly logged!\n");
}

void quit(int exit_code) {
  debug_log(log_INFO, "Quitting app: %i\n", exit_code);

  refresh();
  endwin();
  debug_log(log_INFO, "Closed ncurses");
  printf("ncurses closed\n");

  debug_log(log_INFO, "Exiting");
  printf("Exiting...\n");
  exit(exit_code);
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

int parse_long(const char* str, long* val) {
  char* endptr;

  errno = 0;
  *val = strtol(str, &endptr, 10);

  if (errno != 0) RES_RETURN(r_EPARSE, -1);
  if (str + strlen(str) != endptr) RES_RETURN(r_EPARSE, -1);

  RES_RETURN(r_ENONE, 0);
}

int parse_hex_byte(const char* str, uint8_t* byte) {
	char* endptr;

	if (str == NULL || byte == NULL)
		RES_RETURN(r_EARGS, -1);

	if (strncasecmp(str, "0x", 2) == 0)
		str += 2;

	if (*str == '\0')
		RES_RETURN(r_EPARSE, -1);

	if (strlen(str) > 2)
		RES_RETURN(r_EPARSE, -1);

	errno = 0;

	long value = strtol(str, &endptr, 16);

	if (errno != 0)
		RES_RETURN(r_EPARSE, -1);

	if (*endptr != '\0')
		RES_RETURN(r_EPARSE, -1);

	if (value < 0 || value > 0xFF)
		RES_RETURN(r_EPARSE, -1);

	*byte = (uint8_t)value;

	RES_RETURN(r_ENONE, 0);
}

int parse_hex_word(const char* str, uint16_t* word) {
	char* endptr;

	if (str == NULL || word == NULL)
		RES_RETURN(r_EARGS, -1);

	if (strncasecmp(str, "0x", 2) == 0)
		str += 2;

	if (*str == '\0')
		RES_RETURN(r_EPARSE, -1);

	if (strlen(str) > 4)
		RES_RETURN(r_EPARSE, -1);

	errno = 0;

	long value = strtol(str, &endptr, 16);

	if (errno != 0)
		RES_RETURN(r_EPARSE, -1);

	if (*endptr != '\0')
		RES_RETURN(r_EPARSE, -1);

	if (value < 0 || value > 0xFFFF)
		RES_RETURN(r_EPARSE, -1);

	*word = (uint16_t)value;

	RES_RETURN(r_ENONE, 0);
}

int parse_bool(const char* str, bool* val) {
  if (strcmp(str, "true") == 0) *val = true;
  else if (strcmp(str, "1") == 0) *val = true;
  else if (strcmp(str, "false") == 0) *val = false;
  else if (strcmp(str, "0") == 0) *val = false;
  else {
    RES_RETURN(r_EPARSE, -1);
  }

  RES_RETURN(r_ENONE, 0);
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

  quit(-1);
}

