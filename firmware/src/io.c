#include "io.h"
#include "twi.h"
#include <avr/io.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

static uint8_t iox_set_pin(uint8_t addr, uint8_t reg_addr, uint8_t pin, bool high) {
  if (pin > 7) return 1;

  uint8_t reg;
  if (twi_read_reg(addr, reg_addr, &reg)) return 1;

  if (high) reg |= 1 << pin;
  else reg &= ~(1 << pin);

  if (twi_write_reg(addr, reg_addr, reg)) return 1;
  
  return 0;
}

static uint8_t iox_get_pin(uint8_t addr, uint8_t reg_addr, uint8_t pin, bool* high) {
  if (pin > 7) return 1;
  if (!high) return 1;

  uint8_t reg;
  if (twi_read_reg(addr, reg_addr, &reg)) return 1;

  *high = reg & (1 << pin);

  return 0;
}

uint8_t io_init(void) {
  if (iox_set_pin(IOX1_ADDR, IOX_IODIRA, PIN_EXT_RESETB, IOX_OUTPUT)) return 1;
  if (iox_set_pin(IOX1_ADDR, IOX_IODIRB, PIN_DEV_ENB, IOX_OUTPUT)) return 1;
  if (iox_set_pin(IOX1_ADDR, IOX_IODIRA, PIN_VPB, IOX_INPUT)) return 1;
  if (iox_set_pin(IOX1_ADDR, IOX_IODIRA, PIN_SYNC, IOX_INPUT)) return 1;
  if (iox_set_pin(IOX1_ADDR, IOX_IODIRA, PIN_MLB, IOX_INPUT)) return 1;
  
  if (io_set_reset(false)) return 1;

  DDRD |= 1 << PIN_EXT_CLK;
  io_highz_databus();

  return 0;
}

uint8_t io_set_cpu_en(bool enable) {
  if (enable) {
    io_highz_databus();
    if (io_highz_addrbus()) return 1;
    if (io_highz_rw()) return 1;
    if (io_set_reset(false)) return 1;
  } else {
    if (io_set_ext_clk_en(true)) return 1;
  }

  if (iox_set_pin(IOX1_ADDR, IOX_IODIRA, PIN_EXT_CPU_EN, IOX_OUTPUT)) return 1;
  if (iox_set_pin(IOX1_ADDR, IOX_OLATA, PIN_EXT_CPU_EN, enable)) return 1;

  if (!enable) {
    if (io_set_rw(false)) return 1;
  }

  return 0;
}

uint8_t io_set_ext_clk_en(bool enable) {
  if (iox_set_pin(IOX1_ADDR, IOX_IODIRA, PIN_EXT_CLK_EN, IOX_OUTPUT)) return 1;
  if (iox_set_pin(IOX1_ADDR, IOX_OLATA, PIN_EXT_CLK_EN, enable)) return 1;

  return 0;
}

void io_set_ext_clk(bool high) {
  if (high) {
    PORTD |= 1 << PIN_EXT_CLK;
  } else {
    PORTD &= ~(1 << PIN_EXT_CLK);
  }
}

void io_flip_ext_clk(void) {
  PIND |= 1 << PIN_EXT_CLK;
}

bool io_get_ext_clk(void) {
  bool is_high = PORTD & (1 << PIN_EXT_CLK);

  return is_high ? HIGH : LOW;
}

uint8_t io_set_rw(bool writing) {
  if (iox_set_pin(IOX1_ADDR, IOX_IODIRB, PIN_RWB, IOX_OUTPUT)) return 1;
  if (iox_set_pin(IOX1_ADDR, IOX_OLATB, PIN_RWB, !writing)) return 1;

  return 0;
}

uint8_t io_get_rw(bool* writing) {
  if (iox_set_pin(IOX1_ADDR, IOX_IODIRB, PIN_RWB, IOX_INPUT)) return 1;
  if (iox_get_pin(IOX1_ADDR, IOX_GPIOB, PIN_RWB, writing)) return 1;

  *writing = !(*writing);

  return 0;
}

uint8_t io_highz_rw(void) {
  if (iox_set_pin(IOX1_ADDR, IOX_IODIRB, PIN_RWB, IOX_INPUT)) return 1;

  return 0;
}

uint8_t io_set_dev_en(bool enable) {
  if (iox_set_pin(IOX1_ADDR, IOX_OLATB, PIN_DEV_ENB, !enable)) return 1;

  return 0;
}

uint8_t io_set_reset(bool reset) {
  if (iox_set_pin(IOX1_ADDR, IOX_OLATA, PIN_EXT_RESETB, !reset)) return 1;

  return 0;
}

uint8_t io_get_dev_tbo(bool* owned) {
  if (iox_set_pin(IOX1_ADDR, IOX_IODIRA, PIN_DEV_TBO, IOX_INPUT)) return 1;
  if (iox_get_pin(IOX1_ADDR, IOX_GPIOA, PIN_DEV_TBO, owned)) return 1;

  return 0;
}

uint8_t io_get_vp(bool* pull_vector) {
  if (iox_get_pin(IOX1_ADDR, IOX_GPIOA, PIN_VPB, pull_vector)) return 1;

  *pull_vector = !(*pull_vector);

  return 0;
}

uint8_t io_get_sync(bool* sync) {
  if (iox_get_pin(IOX1_ADDR, IOX_GPIOA, PIN_SYNC, sync)) return 1;

  return 0;
}

uint8_t io_get_ml(bool* memory_block) {
  if (iox_get_pin(IOX1_ADDR, IOX_GPIOA, PIN_MLB, memory_block)) return 1;

  *memory_block = !(*memory_block);

  return 0;
}

uint8_t io_get_nmi(bool* active) {
  if (iox_set_pin(IOX1_ADDR, IOX_IODIRB, PIN_NMIB, IOX_INPUT)) return 1;
  if (iox_get_pin(IOX1_ADDR, IOX_GPIOB, PIN_NMIB, active)) return 1;

  *active = !(*active);

  return 0;
}

uint8_t io_set_nmi(bool active) {
  if (active) {
    if (iox_set_pin(IOX1_ADDR, IOX_IODIRB, PIN_NMIB, IOX_OUTPUT)) return 1;
    if (iox_set_pin(IOX1_ADDR, IOX_GPIOB, PIN_NMIB, LOW)) return 1;
  } else {
    if (iox_set_pin(IOX1_ADDR, IOX_IODIRB, PIN_NMIB, IOX_INPUT)) return 1;
  }

  return 0;
}

uint8_t io_get_irq(bool* active) {
  if (iox_set_pin(IOX1_ADDR, IOX_IODIRB, PIN_IRQB, IOX_INPUT)) return 1;
  if (iox_get_pin(IOX1_ADDR, IOX_GPIOB, PIN_IRQB, active)) return 1;

  *active = !(*active);

  return 0;
}

uint8_t io_set_irq(bool active) {
  if (active) {
    if (iox_set_pin(IOX1_ADDR, IOX_IODIRB, PIN_IRQB, IOX_OUTPUT)) return 1;
    if (iox_set_pin(IOX1_ADDR, IOX_GPIOB, PIN_IRQB, LOW)) return 1;
  } else {
    if (iox_set_pin(IOX1_ADDR, IOX_IODIRB, PIN_IRQB, IOX_INPUT)) return 1;
  }

  return 0;
}

static void databus_set_iodir(bool output) {
  if (output) {
    DDRD |= 0xF0; // Set PD4-PD7
    DDRB |= 0x0F; // Set PB0-PB3
  } else {
    DDRD &= ~(0xF0); // Clear PD4-PD7
    DDRB &= ~(0x0F); // Clear PB0-PB3
    
    PORTD &= ~(0xF0); // Clear PD4-PD7
    PORTB &= ~(0x0F); // Clear PB0-PB3
  }
}

void io_write_databus(uint8_t value) {
  databus_set_iodir(true);

  // Lower nibble (bits 0-3) goes to PORTD (bits 4-7)
  PORTD = (PORTD & 0x0F) | ((value & 0x0F) << 4);
  
  // Upper nibble (bits 4-7) goes to PORTB (bits 0-3)
  PORTB = (PORTB & 0xF0) | ((value & 0xF0) >> 4);
}

uint8_t io_read_databus(void) {
  databus_set_iodir(false);

  uint8_t value = 0;

  // Read PORTD bits 4-7 into bits 0-3 of value
  value |= PIND >> 4;
  
  // Read PORTB bits 0-3 into bits 4-7 of value
  value |= PINB << 4;

  return value;
}

void io_highz_databus(void) {
  databus_set_iodir(false);
}

uint8_t io_read_addrbus(uint16_t* addr) {
  if (twi_write_reg(IOX0_ADDR, IOX_IODIRA, IOX_PORT_INPUT)) return 1;
  if (twi_write_reg(IOX0_ADDR, IOX_IODIRB, IOX_PORT_INPUT)) return 1;

  uint8_t lb;
  uint8_t hb;

  if (twi_read_reg(IOX0_ADDR, IOX_GPIOA, &lb)) return 1;
  if (twi_read_reg(IOX0_ADDR, IOX_GPIOB, &hb)) return 1;

  *addr = (hb << 8) | lb;

  return 0;
}

uint8_t io_write_addrbus(uint16_t addr) {
  if (twi_write_reg(IOX0_ADDR, IOX_IODIRA, IOX_PORT_OUTPUT)) return 1;
  if (twi_write_reg(IOX0_ADDR, IOX_IODIRB, IOX_PORT_OUTPUT)) return 1;

  uint8_t lb = addr & 0xFF;
  uint8_t hb = addr >> 8;

  if (twi_start_addr(IOX0_ADDR, TWI_WRITE)) return 1;
  if (twi_write(IOX_OLATA)) return 1;
  if (twi_write(lb)) return 1;
  if (twi_write(hb)) return 1;
  twi_stop();

  return 0;
}

uint8_t io_highz_addrbus(void) {
  if (twi_write_reg(IOX0_ADDR, IOX_IODIRA, IOX_PORT_INPUT)) return 1;
  if (twi_write_reg(IOX0_ADDR, IOX_IODIRB, IOX_PORT_INPUT)) return 1;

  return 0;
}

