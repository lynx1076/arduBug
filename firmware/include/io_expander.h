#ifndef IO_EXPANDER_H
#define IO_EXPANDER_H

#include "io.h"
#include <stdint.h>
#include <stdbool.h>

#define IOX_0_PA0 0
#define IOX_0_PA1 1
#define IOX_0_PA2 2
#define IOX_0_PA3 3
#define IOX_0_PA4 4
#define IOX_0_PA5 5
#define IOX_0_PA6 6
#define IOX_0_PA7 7

#define IOX_0_PB0 8
#define IOX_0_PB1 9
#define IOX_0_PB2 10
#define IOX_0_PB3 11
#define IOX_0_PB4 12
#define IOX_0_PB5 13
#define IOX_0_PB6 14
#define IOX_0_PB7 15

#define IOX_1_PA0 16
#define IOX_1_PA1 17
#define IOX_1_PA2 18
#define IOX_1_PA3 19
#define IOX_1_PA4 20
#define IOX_1_PA5 21
#define IOX_1_PA6 22
#define IOX_1_PA7 23

#define IOX_1_PB0 24
#define IOX_1_PB1 25
#define IOX_1_PB2 26
#define IOX_1_PB3 27
#define IOX_1_PB4 28
#define IOX_1_PB5 29
#define IOX_1_PB6 30
#define IOX_1_PB7 31

typedef struct {
  uint8_t pin;
} IOXContext;

extern const IOVtable iox_vtable;

uint8_t iox_init(void);
uint8_t iox_flush(void);

#endif

