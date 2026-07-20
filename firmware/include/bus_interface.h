#ifndef BUS_INTERFACE_H
#define BUS_INTERFACE_H


#include <stdbool.h>
#include <stdint.h>

uint8_t bif_mem_read(uint16_t addr, uint8_t* data);
uint8_t bif_mem_write(uint16_t addr, uint8_t data);

uint8_t bif_mem_bulk_read(uint16_t base_addr, uint8_t length, uint8_t* data);
uint8_t bif_mem_bulk_write(uint16_t base_addr, uint8_t length, uint8_t* data);

#endif
