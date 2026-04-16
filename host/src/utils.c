#include "utils.h"
#include <stddef.h>
#include <stdint.h>
#include <time.h>

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

