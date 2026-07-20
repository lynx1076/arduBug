#include "bus_interface.h"
#include "io.h"
#include "twi.h"
#include <stdlib.h>
#include <stdint.h>

uint8_t bif_mem_read(uint16_t addr, uint8_t* data) {
  if (io_set_dev_en(false)) return 1;
  if (io_set_ext_clk_en(true)) return 1;
  if (io_set_cpu_en(false)) return 1;
  io_set_ext_clk(LOW);

  if (io_write_addrbus(addr)) return 1;
  io_highz_databus();

  if (io_set_rw(false)) return 1;
  if (io_set_dev_en(true)) return 1;
  io_set_ext_clk(HIGH);

  bool bus_owned;
  if (io_get_dev_tbo(&bus_owned)) return 1;
  if (!bus_owned) return 1;

  *data = io_read_databus();
  if (io_set_dev_en(true)) return 1;

  return 0;
}

uint8_t bif_mem_write(uint16_t addr, uint8_t data) {
  io_highz_databus();
  if (io_highz_addrbus()) return 1;
  if (io_set_dev_en(false)) return 1;
  if (io_set_ext_clk_en(true)) return 1;
  if (io_set_cpu_en(false)) return 1;
  io_set_ext_clk(HIGH);

  if (io_write_addrbus(addr)) return 1;

  io_write_databus(data);

  if (io_set_rw(true)) return 1;

  if (io_set_dev_en(true)) return 1;
  if (io_set_dev_en(false)) return 1;

  if (io_set_rw(false)) return 1;

  return 0;
}

uint8_t bif_mem_bulk_read(uint16_t base_addr, uint8_t length, uint8_t* data) {
  if (data == NULL) return 1;
  if (length == 0) return 0;

  io_highz_databus();
  if (io_highz_addrbus()) return 1;
  if (io_set_dev_en(false)) return 1;
  if (io_set_ext_clk_en(true)) return 1;
  if (io_set_cpu_en(false)) return 1;
  io_set_ext_clk(HIGH);
  if (io_set_rw(false)) return 1;
  if (io_set_dev_en(true)) return 1;

  // Set the address to output
  if (twi_write_reg(IOX0_ADDR, IOX_IODIRA, IOX_PORT_OUTPUT)) return 1;
  if (twi_write_reg(IOX0_ADDR, IOX_IODIRB, IOX_PORT_OUTPUT)) return 1;

  for (uint8_t i = 0; i < length; i++) {
    // Stream 16-bit address in single TWI burst
    uint16_t addr = base_addr + i;
    if (twi_start_addr(IOX0_ADDR, TWI_WRITE)) return 1;
    if (twi_write(IOX_OLATA)) return 1;
    if (twi_write((uint8_t)(addr & 0xFF))) return 1; // OLATA
    if (twi_write((uint8_t)(addr >> 8))) return 1; // Auto-increment to OLATB
    twi_stop();

    data[i] = io_read_databus();
  }

  return 0;
}

uint8_t bif_mem_bulk_write(uint16_t base_addr, uint8_t length, uint8_t* data) {
  if (data == NULL) return 1;
  if (length == 0) return 0;

  io_highz_databus();
  if (io_highz_addrbus()) return 1;
  if (io_set_dev_en(false)) return 1;
  if (io_set_ext_clk_en(true)) return 1;
  if (io_set_cpu_en(false)) return 1;
  io_set_ext_clk(LOW);
  if (io_set_rw(true)) return 1;
  if (io_set_dev_en(true)) return 1;

  // Set the address to output
  if (twi_write_reg(IOX0_ADDR, IOX_IODIRA, IOX_PORT_OUTPUT)) return 1;
  if (twi_write_reg(IOX0_ADDR, IOX_IODIRB, IOX_PORT_OUTPUT)) return 1;

  for (uint8_t i = 0; i < length; i++) {
    // Load byte onto native pins
    io_write_databus(data[i]);

    // Stream 16-bit address in single TWI burst
    uint16_t addr = base_addr + i;
    if (twi_start_addr(IOX0_ADDR, TWI_WRITE)) return 1;
    if (twi_write(IOX_OLATA)) return 1;
    if (twi_write((uint8_t)(addr & 0xFF))) return 1; // OLATA
    if (twi_write((uint8_t)(addr >> 8))) return 1; // Auto-increment to OLATB
    twi_stop();

    io_set_ext_clk(HIGH);
    io_set_ext_clk(LOW);
  }

  return 0;
}

