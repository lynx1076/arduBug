#ifndef TWI_H
#define TWI_H

#include <avr/io.h>

#define TWI_READ            1
#define TWI_WRITE           0

void twi_init(void);
uint8_t twi_start(void);
void twi_stop(void);

uint8_t twi_write(uint8_t data);
uint8_t twi_read_ack(uint8_t *data);
uint8_t twi_read_nack(uint8_t *data);

uint8_t twi_start_addr(uint8_t addr, uint8_t mode);
uint8_t twi_check_device_present(uint8_t addr);

uint8_t twi_write_reg(uint8_t addr, uint8_t reg, uint8_t data);
uint8_t twi_read_reg(uint8_t addr, uint8_t reg, uint8_t* data);

#endif
