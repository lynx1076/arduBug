#ifndef BUS_INTERFACE_H
#define BUS_INTERFACE_H


#include "io.h"
#include <stdbool.h>
#include <stdint.h>

uint8_t bif_read_databus(uint8_t* data);
uint8_t bif_read_addrbus(uint16_t* addr);

uint8_t bif_read_rw(bool* writing);

uint8_t bif_read_dev_tbo(bool* bus_owned);
uint8_t bif_read_sync(bool* sync);
uint8_t bif_read_ml(bool* memory_locked);
uint8_t bif_read_vp(bool* fetch_vector);

uint8_t bif_read_irq(bool* active);
uint8_t bif_read_nmi(bool* active);
uint8_t bif_set_irq(bool active);
uint8_t bif_set_nmi(bool active);

uint8_t bif_set_ext_clk(IOLevel level);

uint8_t bif_set_dev_en(bool device_enable);
uint8_t bif_set_cpu_en(bool enable);
uint8_t bif_set_ext_clk_en(bool enable);

uint8_t bif_mem_read(uint16_t addr, uint8_t* data);
uint8_t bif_mem_write(uint16_t addr, uint8_t data);

#endif
