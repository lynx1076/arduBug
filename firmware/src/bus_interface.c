#include "bus_interface.h"
#include "io.h"
#include "io_defs.h"
#include <stdint.h>
#include <util/delay.h>

static uint8_t bif_highz_databus(void);
static uint8_t bif_highz_addrbus(void);
static uint8_t bif_write_databus(uint8_t data);
static uint8_t bif_write_addrbus(uint16_t addr);

static uint8_t bif_set_rw(bool writing);
static uint8_t bif_highz_rw(void);

static uint8_t bif_highz_databus(void) {
  for (uint8_t i = 0; i < 8; i++) {
    if (io_highz(bus_data[i])) return 1;
  }

  return 0;
}

static uint8_t bif_highz_addrbus(void) {
  for (uint8_t i = 0; i < 16; i++) {
    if (io_highz(bus_addr[i])) return 1;
  }

  return 0;
}

uint8_t bif_read_databus(uint8_t* data) {
  if (!data) return 1;

  uint8_t data_buf = 0;

  for (uint8_t i = 0; i < 8; i++) {
    IOLevel state;
    if (io_read(bus_data[i], &state)) return 1;

    data_buf |= (state == io_HIGH ? 1 : 0) << i;
  }

  *data = data_buf;

  return 0;
}

uint8_t bif_read_addrbus(uint16_t* addr) {
  if (!addr) return 1;

  uint16_t addr_buf = 0;

  for (uint8_t i = 0; i < 16; i++) {
    IOLevel state;
    if (io_read(bus_addr[i], &state)) return 1;

    addr_buf |= (state == io_HIGH ? 1 : 0) << i;
  }

  *addr = addr_buf;

  return 0;
}

static uint8_t bif_write_databus(uint8_t data) {
  for (uint8_t i = 0; i < 8; i++) {
    IOLevel state = data & (1 << i) ? io_HIGH : io_LOW;
    if (io_write(bus_data[i], state)) return 1;
  }

  return 0;
}

static uint8_t bif_write_addrbus(uint16_t addr) {
  for (uint8_t i = 0; i < 16; i++) {
    IOLevel state = addr & (1 << i) ? io_HIGH : io_LOW;
    if (io_write(bus_addr[i], state)) return 1;
  }

  return 0;
}

uint8_t bif_read_rw(bool* writing) {
  IOLevel io_rwb;

  if (io_read(&pin_RWB, &io_rwb)) return 1;

  *writing = io_rwb == io_LOW;

  return 0;
}

static uint8_t bif_set_rw(bool writing) {
  IOLevel io_rwb = writing ? io_LOW : io_HIGH;

  if (io_write(&pin_RWB, io_rwb)) return 1;

  return 0;
}

static uint8_t bif_highz_rw(void) {
  if (io_highz(&pin_RWB)) return 1;

  return 0;
}

uint8_t bif_read_dev_tbo(bool* bus_owned) {
  IOLevel io_tbo;

  if (io_read(&pin_DEV_TBO, &io_tbo)) return 1;

  *bus_owned = io_tbo == io_HIGH;

  return 0;
}

uint8_t bif_read_sync(bool* sync) {
  IOLevel io_sync;

  if (io_read(&pin_SYNC, &io_sync)) return 1;

  *sync = io_sync == io_HIGH;

  return 0;
}

uint8_t bif_read_ml(bool* memory_locked) {
  IOLevel io_mlb;

  if (io_read(&pin_MLB, &io_mlb)) return 1;

  *memory_locked = io_mlb == io_LOW;

  return 0;
}

uint8_t bif_read_vp(bool* fetch_vector) {
  IOLevel io_vpb;

  if (io_read(&pin_VPB, &io_vpb)) return 1;

  *fetch_vector = io_vpb == io_LOW;

  return 0;
}

uint8_t bif_read_irq(bool* active) {
  IOLevel io_irqb;

  if (io_read(&pin_IRQB, &io_irqb)) return 1;

  *active = io_irqb == io_LOW;

  return 0;
}

uint8_t bif_read_nmi(bool* active) {
  IOLevel io_nmib;

  if (io_read(&pin_NMIB, &io_nmib)) return 1;

  *active = io_nmib == io_LOW;

  return 0;
}

uint8_t bif_set_irq(bool active) {
  if (active) {
    if (io_write(&pin_IRQB, io_LOW)) return 1;
  } else {
    if (io_highz(&pin_IRQB)) return 1;
  }

  return 0;
}

uint8_t bif_set_nmi(bool active) {
  if (active) {
    if (io_write(&pin_NMIB, io_LOW)) return 1;
  } else {
    if (io_highz(&pin_NMIB)) return 1;
  }

  return 0;
}

uint8_t bif_set_dev_en(bool enable) {
  IOLevel io_dev_enb = enable ? io_LOW : io_HIGH;

  if (io_write(&pin_DEV_ENB, io_dev_enb)) return 1;

  return 0;
}

uint8_t bif_set_cpu_en(bool enable)  {
  IOLevel io_cpu_en = enable ? io_HIGH : io_LOW;

  if (enable) {
    if (io_write(&pin_EXT_CPU_EN, io_cpu_en)) return 1;
    if (bif_set_rw(false)) return 1;
    if (bif_highz_databus()) return 1;
    if (bif_write_addrbus(0x0000)) return 1;
  } else {
    if (bif_highz_databus()) return 1;
    if (bif_highz_addrbus()) return 1;
    if (bif_highz_rw()) return 1;
    if (io_write(&pin_EXT_CPU_EN, io_cpu_en)) return 1;
  }

  return 0;
}

uint8_t bif_set_ext_clk_en(bool enable) {
  IOLevel io_ext_clk_en = enable ? io_HIGH : io_LOW;

  if (io_write(&pin_EXT_CLK_EN, io_ext_clk_en)) return 1;

  return 0;
}

uint8_t bif_set_ext_clk(IOLevel level) {
  if (io_write(&pin_EXT_CLK, level)) return 1;

  return 0;
}

uint8_t bif_mem_read(uint16_t addr, uint8_t* data) {
  if (bif_set_dev_en(false)) return 1;
  if (bif_set_ext_clk_en(true)) return 1;
  if (bif_set_cpu_en(false)) return 1;
  if (bif_set_ext_clk(io_LOW)) return 1;

  if (bif_write_addrbus(addr)) return 1;
  if (bif_highz_databus()) return 1;

  if (bif_set_rw(false)) return 1;
  if (bif_set_dev_en(true)) return 1;
  if (bif_set_ext_clk(io_HIGH)) return 1;

  bool bus_owned;
  if (bif_read_dev_tbo(&bus_owned)) return 1;
  if (!bus_owned) return 1;

  if (bif_read_databus(data)) return 1;
  if (bif_set_dev_en(true)) return 1;

  return 0;
}

uint8_t bif_mem_write(uint16_t addr, uint8_t data) {
  if (bif_set_dev_en(false)) return 1;
  if (bif_set_ext_clk_en(true)) return 1;
  if (bif_set_cpu_en(false)) return 1;
  if (bif_set_ext_clk(io_HIGH)) return 1;

  if (bif_write_addrbus(addr)) return 1;

  if (bif_write_databus(data)) return 1;

  if (bif_set_rw(true)) return 1;

  if (bif_set_dev_en(true)) return 1;
  if (bif_set_dev_en(false)) return 1;

  if (bif_set_rw(false)) return 1;

  return 0;
}

