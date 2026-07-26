#ifndef DEVICE_H
#define DEVICE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define LOW       0
#define HIGH      1

int dev_init(void);
int dev_ping(void);
int dev_set_ext_clk_en(bool enable);
bool dev_get_ext_clk_en(void);
int dev_set_ext_clk(bool is_high);
bool dev_get_ext_clk(void);
int dev_set_cpu_en(bool enable);
bool dev_get_cpu_en(void);
int dev_get_cpu_state(uint8_t* state);
int dev_print_bus_state(void);
int dev_read_databus(uint8_t* data);
int dev_read_addrbus(uint16_t* addr);
int dev_set_reset(bool reset);
bool dev_get_reset(void);
int dev_reset_cpu(void);
long dev_step_clock(long steps);
int dev_step_instructions(long instructions);
int dev_mem_read(uint16_t addr, uint8_t* data);
int dev_mem_write(uint16_t addr, uint8_t data);
int dev_mem_bulk_read(uint16_t addr, uint8_t count, uint8_t* data);

#endif
