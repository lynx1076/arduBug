#include "twi.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

#define TWI_START           0x08 // START condition transmitted
#define TWI_REP_START       0x10 // Repeated START transmitted
#define TWI_MT_SLA_ACK      0x18 // SLA+W transmitted, ACK received
#define TWI_MT_SLA_NACK     0x20 // SLA+W transmitted, NACK received
#define TWI_MT_DATA_ACK     0x28 // Data transmitted, ACK received
#define TWI_MT_DATA_NACK    0x30 // Data transmitted, NACK received

#define TWI_MR_SLA_ACK      0x40 // SLA+R transmitted, ACK received
#define TWI_MR_SLA_NACK     0x48 // SLA+R transmitted, NACK received
#define TWI_MR_DATA_ACK     0x50 // Data received, ACK returned
#define TWI_MR_DATA_NACK    0x58 // Data received, NACK returned

#define TWI_TIMEOUT_MS 200

#define TWI_PIN_SDA         PC4
#define TWI_PIN_SCL         PC5

static uint8_t twi_wait_twint(void) {
  uint16_t elapsed = 0;
  while (!(TWCR & _BV(TWINT))) {
    _delay_ms(1);
    if (elapsed++ >= TWI_TIMEOUT_MS) {
      return 1;
    }
  }

  return 0;
}

void twi_init(void) {
  TWBR = 72;
  TWSR = 0x00;
  TWCR = _BV(TWEN);
}

uint8_t twi_start(void) {
  TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
  if (twi_wait_twint()) return 1;
  uint8_t status = TWSR & 0xF8;
  return (status != TWI_START && status != TWI_REP_START);
}

void twi_stop(void) {
  TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWSTO);
  // Wait for STOP condition to complete
  while (TWCR & _BV(TWSTO));
}

uint8_t twi_write(uint8_t data) {
  TWDR = data;
  TWCR = _BV(TWINT) | _BV(TWEN);
  if (twi_wait_twint()) return 1;
  uint8_t status = TWSR & 0xF8;
  return (status != TWI_MT_SLA_ACK && status != TWI_MT_DATA_ACK && status != TWI_MR_SLA_ACK);
}

uint8_t twi_read_ack(uint8_t *data) {
  TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWEA);
  if (twi_wait_twint()) return 1;
  *data = TWDR;
  uint8_t status = TWSR & 0xF8;
  return (status != TWI_MR_DATA_ACK);
}

uint8_t twi_read_nack(uint8_t *data) {
  TWCR = _BV(TWINT) | _BV(TWEN);
  if (twi_wait_twint()) return 1;
  *data = TWDR;
  uint8_t status = TWSR & 0xF8;
  return (status != TWI_MR_DATA_NACK);
}

uint8_t twi_start_addr(uint8_t addr, uint8_t mode) {
  if (twi_start()) return 1;
  return twi_write(addr << 1 | (mode & 0x01));
}

uint8_t twi_check_device_present(uint8_t addr) {
  uint8_t state = twi_start_addr(addr, TWI_READ);
  twi_stop();
  return state;
}

uint8_t twi_write_reg(uint8_t addr, uint8_t reg, uint8_t data) {
  if (twi_start_addr(addr, TWI_WRITE)) goto ERROR;
  if (twi_write(reg)) goto ERROR;
  if (twi_write(data)) goto ERROR;
  twi_stop();

  return 0;

ERROR:
  twi_stop();
  return 1;
}

uint8_t twi_read_reg(uint8_t addr, uint8_t reg, uint8_t* data) {
  if (twi_start_addr(addr, TWI_WRITE)) goto ERROR;
  if (twi_write(reg)) goto ERROR;
  if (twi_start_addr(addr, TWI_READ)) goto ERROR;
  if (twi_read_nack(data)) goto ERROR;
  twi_stop();

  return 0;

ERROR:
  twi_stop();
  return 1;
}

