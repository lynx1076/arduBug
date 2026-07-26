#include "device.h"
#include "serial.h"
#include "serial_protocol.h"
#include "result.h"
#include "gui.h"
#include "ucobs.h"
#include <raylib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define MAX_CYCLES_BEFORE_OPCODE        8

static bool ext_clk_en = true;
static bool ext_clk = LOW;
static bool cpu_en = true;

int dev_init(void) {
  if (dev_set_ext_clk_en(true)) return -1;
  if (dev_set_ext_clk(LOW)) return -1;
  if (dev_set_cpu_en(true)) return -1;

  RES_RETURN(r_ENONE, 0);
}

int dev_set_ext_clk_en(bool enable) {
  if (!ser_is_ready()) RES_RETURN(r_EDEVICE, -1);

  if (ser_enc_write_va(2, SP_CMD_SET_EXT_CLOCK_EN, enable)) RES_RETURN(r_EDEVICE, -1);

  uint8_t reply;
  if (ser_enc_read_va(1, &reply)) return -1;

  if (reply != SP_SIG_OK) RES_RETURN(r_EDEVICE, -1);

  ext_clk_en = enable;

  RES_RETURN(r_ENONE, 0);
}

int dev_set_ext_clk(bool is_high) {
  if (!ser_is_ready()) RES_RETURN(r_EDEVICE, -1);

  if (ser_enc_write_va(2, SP_CMD_SET_EXT_CLOCK, is_high)) RES_RETURN(r_EDEVICE, -1);

  uint8_t reply;
  if (ser_enc_read_va(1, &reply)) return -1;

  if (reply != SP_SIG_OK) RES_RETURN(r_EDEVICE, -1);

  ext_clk = is_high;

  RES_RETURN(r_ENONE, 0);
}

bool dev_get_ext_clk_en(void) {
  return ext_clk_en;
}

bool dev_get_ext_clk(void) {
  return ext_clk;
}

int dev_set_cpu_en(bool enable) {
  if (!ser_is_ready()) RES_RETURN(r_EDEVICE, -1);

  if (ser_enc_write_va(2, SP_CMD_SET_CPU_EN, enable)) RES_RETURN(r_EDEVICE, -1);

  uint8_t reply;
  if (ser_enc_read_va(1, &reply)) return -1;

  if (reply != SP_SIG_OK) RES_RETURN(r_EDEVICE, -1);

  cpu_en = enable;

  RES_RETURN(r_ENONE, 0);
}

bool dev_get_cpu_en(void) {
  return cpu_en;
}

int dev_get_cpu_state(uint8_t* state) {
  if (!ser_is_ready()) RES_RETURN(r_EDEVICE, -1);
  if (state == NULL) RES_RETURN(r_ENULL_PTR, -1);

  if (ser_enc_write_va(1, SP_CMD_GET_CPU_STATE)) RES_RETURN(r_EDEVICE, -1);

  uint8_t return_code;
  if (ser_enc_read_va(2, &return_code, state)) return -1;

  if (return_code != SP_SIG_OK) RES_RETURN(r_EDEVICE, -1);

  RES_RETURN(r_ENONE, 0);
}

int dev_print_bus_state(void) {
  if (!ser_is_ready()) RES_RETURN(r_EDEVICE, -1);

  uint8_t cpu_state;
  if (dev_get_cpu_state(&cpu_state)) return -1;

  uint8_t data;
  if (dev_read_databus(&data)) return -1;

  uint16_t addr;
  if (dev_read_addrbus(&addr)) return -1;

  bool writing = cpu_state & (1 << SP_STATE_WRITING);

  if (!(cpu_state & (1 << SP_STATE_DEV_TBO))) gui_log("The bus is not owned");
  if (cpu_state & (1 << SP_STATE_ML)) gui_log("The memory is locked");
  if (cpu_state & (1 << SP_STATE_VP)) gui_log("The CPU is fetching a vector");
  if (cpu_state & (1 << SP_STATE_SYNC)) gui_log("The CPU is fetching an opcode");
  if (cpu_state & (1 << SP_STATE_IRQ)) gui_log("IRQ is active");
  if (cpu_state & (1 << SP_STATE_NMI)) gui_log("NMI is active");

  gui_log(TextFormat("CPU is %s 0x%02x at 0x%04x", writing ? "writing" : "reading", data, addr));

  RES_RETURN(r_ENONE, 0);
}

int dev_read_databus(uint8_t* data) {
  if (!ser_is_ready()) RES_RETURN(r_EDEVICE, -1);
  if (data == NULL) RES_RETURN(r_ENULL_PTR, -1);
  
  if (ser_enc_write_va(1, SP_CMD_READ_DATABUS)) RES_RETURN(r_EDEVICE, -1);

  uint8_t return_code;
  if (ser_enc_read_va(2, &return_code, data)) return -1;

  if (return_code != SP_SIG_OK) RES_RETURN(r_EDEVICE, -1);

  RES_RETURN(r_ENONE, 0);
}

int dev_read_addrbus(uint16_t* addr) {
  if (!ser_is_ready()) RES_RETURN(r_EDEVICE, -1);
  if (addr == NULL) RES_RETURN(r_ENULL_PTR, -1);
  
  if (ser_enc_write_va(1, SP_CMD_READ_ADDRBUS)) RES_RETURN(r_EDEVICE, -1);

  uint8_t return_code;
  uint8_t hb, lb;

  if (ser_enc_read_va(3, &return_code, &lb, &hb)) return -1;

  if (return_code != SP_SIG_OK) RES_RETURN(r_EDEVICE, -1);

  *addr = (hb << 8) | lb;

  RES_RETURN(r_ENONE, 0);
}

long dev_step_clock(long steps) {
  if (!ser_is_ready()) RES_RETURN(r_EDEVICE, -1);
  if (dev_set_ext_clk_en(true)) return -1;

  for (long i = 0; i < steps; i++) {
    if (dev_set_ext_clk(LOW)) return i;
    if (dev_set_ext_clk(HIGH)) return i;
  }

  RES_RETURN(r_ENONE, steps);
}

int dev_step_instructions(long instructions) {
  if (!ser_is_ready()) RES_RETURN(r_EDEVICE, -1);

  if (dev_set_ext_clk_en(true)) return -1;

  for (long i = 0; i < instructions; i++) {
    int cycles = 0;

    while (true) {
      if (dev_set_ext_clk(LOW)) return i;
      if (dev_set_ext_clk(HIGH)) return i;

      uint8_t state;
      if (dev_get_cpu_state(&state)) return i;
      if (state & (1 << SP_STATE_SYNC)) break;
      
      if (cycles >= MAX_CYCLES_BEFORE_OPCODE) RES_RETURN(r_EDEVICE, i);
      cycles++;
    }
  }

  RES_RETURN(r_ENONE, instructions);
}

int dev_mem_read(uint16_t addr, uint8_t* data) {
  if (!ser_is_ready()) RES_RETURN(r_EDEVICE, -1);
  if (data == NULL) RES_RETURN(r_ENULL_PTR, -1);

  bool _cpu_en = cpu_en;
 
  uint8_t hb = addr >> 8;
  uint8_t lb = addr & 0xFF;

  cpu_en = false;

  if (ser_enc_write_va(3, SP_CMD_MEM_READ, lb, hb)) RES_RETURN(r_EDEVICE, -1);

  uint8_t return_code;
  if (ser_enc_read_va(2, &return_code, data)) return -1;

  if (return_code != SP_SIG_OK) RES_RETURN(r_EDEVICE, -1);

  if (dev_set_cpu_en(_cpu_en)) return -1;

  RES_RETURN(r_ENONE, 0);
}

int dev_mem_write(uint16_t addr, uint8_t data) {
  if (!ser_is_ready()) RES_RETURN(r_EDEVICE, -1);

  bool _cpu_en = cpu_en;
 
  uint8_t hb = addr >> 8;
  uint8_t lb = addr & 0xFF;

  cpu_en = false;

  if (ser_enc_write_va(4, SP_CMD_MEM_WRITE, lb, hb, data)) return -1;

  uint8_t return_code;
  if (ser_enc_read_va(1, &return_code)) return -1;

  if (return_code != SP_SIG_OK) RES_RETURN(r_EDEVICE, -1);

  if (dev_set_cpu_en(_cpu_en)) return -1;

  RES_RETURN(r_ENONE, 0);
}

int dev_mem_bulk_read(uint16_t addr, uint8_t count, uint8_t* data) {
  if (!ser_is_ready()) RES_RETURN(r_EDEVICE, -1);
  if (data == NULL) RES_RETURN(r_ENULL_PTR, -1);

  bool _cpu_en = cpu_en;
 
  uint8_t hb = addr >> 8;
  uint8_t lb = addr & 0xFF;

  cpu_en = false;

  if (ser_enc_write_va(4, SP_CMD_MEM_BULK_READ, lb, hb, count)) RES_RETURN(r_EDEVICE, -1);

  uint8_t reply_len = count + 3;
  uint8_t reply[UCOBS_MAX_DATA_LEN];
  if (ser_enc_read(&reply_len, reply)) return -1;

  uint8_t return_code = reply[0];
  if (return_code != SP_SIG_OK) RES_RETURN(r_EDEVICE, -1);
  memcpy(data, reply + 1, count);

  if (dev_set_cpu_en(_cpu_en)) return -1;

  RES_RETURN(r_ENONE, 0);
}

