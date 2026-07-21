#ifndef IO_H
#define IO_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define LOW       0
#define HIGH      1

int io_init(void);
int io_set_ext_clk_en(bool enable);
int io_set_ext_clk(bool high);
int io_get_cpu_state(uint8_t* state);

#endif
