#ifndef DEVMEM_H
#define DEVMEM_H

#include <stdint.h>

int devm_dump_to_file(const char* filepath);
int devm_flash_from_file(const char* filepath);
int devm_bulk_write(uint16_t addr, uint16_t len, const uint8_t* data);
int devm_update(void);

#endif
