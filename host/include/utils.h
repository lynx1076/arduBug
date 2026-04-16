#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include <stdint.h>


void sleep_us(size_t microseconds);
void sleep_ms(size_t milliseconds);
uint64_t micros(void);
uint64_t millis(void);

#endif
