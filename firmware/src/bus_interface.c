#include "bus_interface.h"
#include "io.h"
#include "serial_protocol.h"
#include "twi.h"
#include "common.h"
#include <util/delay.h>
#include <stdlib.h>
#include <stdint.h>


#define TIMEOUT_WRITE_US    10000

static uint8_t await_write_success(uint8_t data) {
  uint16_t timeout = 0;
  while (timeout < TIMEOUT_WRITE_US) {
    uint8_t read_back = io_read_databus();
    if (read_back == data) break;
    timeout++;
    _delay_us(1);
  }
  if (timeout >= TIMEOUT_WRITE_US) return 1;

  return 0;
}

uint8_t bif_step_instruction(void) {
  bool sync;
  uint8_t cycles = 0;

  do {
    if (cycles >= MAX_CYCLES_BEFORE_OPCODE) return 1;

    io_set_ext_clk(LOW);
    io_set_ext_clk(HIGH);

    if (io_get_sync(&sync)) return 1;

    cycles++;
  } while (!sync);

  return 0;
}

uint8_t bif_mem_read(uint16_t addr, uint8_t* data) {
  bool clk = io_get_ext_clk();

  if (io_set_dev_en(false)) return 1;
  if (io_set_ext_clk_en(true)) return 1;
  if (io_set_cpu_en(false)) return 1;
  io_set_ext_clk(LOW);

  if (io_write_addrbus(addr)) return 1;
  io_highz_databus();

  if (io_set_rw(false)) return 1;
  if (io_set_dev_en(true)) return 1;
  io_set_ext_clk(HIGH);

  *data = io_read_databus();
  if (io_set_dev_en(true)) return 1;

  io_set_ext_clk(clk);

  return 0;
}

uint8_t bif_mem_write(uint16_t addr, uint8_t data) {
  bool clk = io_get_ext_clk();

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
  
  if (io_set_rw(false)) return 1;

  if (await_write_success(data)) return 1;

  io_set_ext_clk(clk);

  return 0;
}

uint8_t bif_mem_bulk_read(uint16_t base_addr, uint8_t length, uint8_t* data) {
  bool clk = io_get_ext_clk();

  if (data == NULL) return 1;
  if (length == 0) return 0;
  if (length > PAGE_SIZE) return 1;
  if (LAST_ADDR - base_addr + 1 < length) return 1;

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

  io_set_ext_clk(clk);

  return 0;
}

uint8_t bif_mem_bulk_write(uint16_t base_addr, uint8_t length, const uint8_t* data) {
  bool clk = io_get_ext_clk();

  if (data == NULL) return 1;
  if (length == 0) return 0;
  if (length > PAGE_SIZE) return 1;
  if (LAST_ADDR - base_addr + 1 < length) return 1;

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

  if (await_write_success(data[length - 1])) return 1;

  io_set_ext_clk(clk);

  return 0;
}

uint8_t bif_get_cpu_state(void) {
  uint8_t state = 0;

  bool tbo;
  if (io_get_dev_tbo(&tbo)) return 1 << SP_STATE_ERROR;
  
  bool vp;
  if (io_get_vp(&vp)) return 1 << SP_STATE_ERROR;
  
  bool sync;
  if (io_get_sync(&sync)) return 1 << SP_STATE_ERROR;

  bool ml;
  if (io_get_ml(&ml)) return 1 << SP_STATE_ERROR;

  bool writing;
  if (io_get_rw(&writing)) return 1 << SP_STATE_ERROR;

  bool nmi;
  if (io_get_nmi(&nmi)) return 1 << SP_STATE_ERROR;

  bool irq;
  if (io_get_irq(&irq)) return 1 << SP_STATE_ERROR;

  state |= tbo ? 1 << SP_STATE_DEV_TBO : 0;
  state |= vp ? 1 << SP_STATE_VP : 0;
  state |= sync ? 1 << SP_STATE_SYNC : 0;
  state |= ml ? 1 << SP_STATE_ML : 0;
  state |= writing ? 1 << SP_STATE_WRITING : 0;
  state |= nmi ? 1 << SP_STATE_NMI : 0;
  state |= irq ? 1 << SP_STATE_IRQ : 0;

  return state;
}

uint8_t bif_rom_permanent_sdp_disable(void) {
  io_highz_databus();
  if (io_highz_addrbus()) return 1;
  if (io_set_dev_en(false)) return 1;
  if (io_set_ext_clk_en(true)) return 1;
  if (io_set_cpu_en(false)) return 1;
  io_set_ext_clk(LOW);
  if (io_set_rw(true)) return 1; // Force active write mode
  if (io_set_dev_en(true)) return 1; // Force ROM Chip Select Active

  // Set address bus expander ports to output
  if (twi_write_reg(IOX0_ADDR, IOX_IODIRA, IOX_PORT_OUTPUT)) return 1;
  if (twi_write_reg(IOX0_ADDR, IOX_IODIRB, IOX_PORT_OUTPUT)) return 1;

  // Fast, inline writes
  #define SDP_RAW_WRITE(addr, byte_val) do { \
    io_write_databus(byte_val); \
    if (twi_start_addr(IOX0_ADDR, TWI_WRITE)) return 1; \
    if (twi_write(IOX_OLATA)) return 1; \
    if (twi_write((uint8_t)((addr) & 0xFF))) return 1; \
    if (twi_write((uint8_t)((addr) >> 8))) return 1; \
    twi_stop(); \
    PORTD &= ~(1 << PD2); \
    PORTD |= (1 << PD2); \
  } while(0)

  // 6-byte sequence
  SDP_RAW_WRITE(0x5555, 0xAA); // Cycle 1
  SDP_RAW_WRITE(0x2AAA, 0x55); // Cycle 2
  SDP_RAW_WRITE(0x5555, 0x80); // Cycle 3
  SDP_RAW_WRITE(0x5555, 0xAA); // Cycle 4
  SDP_RAW_WRITE(0x2AAA, 0x55); // Cycle 5
  SDP_RAW_WRITE(0x5555, 0x20); // Cycle 6

  _delay_ms(20); // t_WC

  // Clean up the bus
  if (io_set_dev_en(false)) return 1;
  if (io_set_rw(false)) return 1;

  return 0;
}

