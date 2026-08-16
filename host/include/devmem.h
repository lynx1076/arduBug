#ifndef DEVMEM_H
#define DEVMEM_H

int devm_dump_to_file(const char* filepath);
int devm_flash_from_file(const char* filepath);
int devm_update(void);

#endif
