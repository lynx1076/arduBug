#include "device.h"
#include "common.h"
#include "serial.h"
#include "serial_protocol.h"
#include "result.h"
#include "gui.h"
#include "ucobs.h"
#include "utils.h"
#include <ncurses.h>
#include <raylib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEVICE_READY_TIMEOUT_MS         2500

static bool ext_clk_en = true;
static bool ext_clk = LOW;
static bool cpu_en = true;
static bool reset_cpu = false;

static size_t device_open_timer_ms = 0;
static bool device_ready = false;

int dev_init(void) {
  if (dev_set_ext_clk_en(true)) return -1;
  if (dev_set_ext_clk(LOW)) return -1;
  if (dev_set_cpu_en(true)) return -1;
  if (dev_set_reset(false)) return -1;

  RES_RETURN(r_ENONE, 0);
}

int dev_update(void) {
  if (!ser_is_open()) {
    device_ready = false;
    device_open_timer_ms = millis();
  } else if (!device_ready) {
    uint8_t compat_code;
    if (!dev_get_compat_code(&compat_code)) {
      if (compat_code != SP_COMPAT_CODE) {
        gui_log("Compat code mismatch!");
        gui_log(TextFormat("Host: 0x%02x != device: 0x%02x", SP_COMPAT_CODE, compat_code));
        ser_close();
        RES_RETURN(r_ECOMPATIBILITY, -1);
      }
      device_ready = true;
      if (dev_init()) {
        ser_close();
        gui_log(TextFormat("Failed to init device: %s", res_get_string(_res)));
        device_ready = false;
        return -1;
      }

      gui_log("Device initialized");
    } else if (millis() - device_open_timer_ms > DEVICE_READY_TIMEOUT_MS) {
      ser_close();
      gui_log(TextFormat("Failed to init device: %s", res_get_string(_res)));
      device_ready = false;
      RES_RETURN(r_ETIMEOUT, -1);
    }
  }

  RES_RETURN(r_ENONE, 0);
}

bool dev_is_ready(void) {
  return device_ready;
}

int dev_ping(void) {
  if (!ser_is_open()) {
    RES_RETURN(r_ENOT_CONNECTED, -1);
  }

  if (ser_enc_write_va(1, SP_CMD_PING)) return -1;

  uint8_t reply;
  if (ser_enc_read_va(1, &reply)) return -1;
  if (reply != SP_SIG_OK) {
    RES_RETURN(r_EDEVICE, -1);
  }

  RES_RETURN(r_ENONE, 0);
}

int dev_get_compat_code(uint8_t* code) {
  if (!ser_is_open()) {
    RES_RETURN(r_ENOT_CONNECTED, -1);
  }

  if (ser_enc_write_va(1, SP_CMD_COMPAT_CODE)) return -1;

  uint8_t reply;
  uint8_t _code;
  if (ser_enc_read_va(2, &reply, &_code)) return -1;
  if (reply != SP_SIG_OK) {
    RES_RETURN(r_EDEVICE, -1);
  }

  if (code) *code = _code;

  RES_RETURN(r_ENONE, 0);
}

int dev_set_ext_clk_en(bool enable) {
  if (!dev_is_ready()) RES_RETURN(r_ENOT_CONNECTED, -1);

  if (ser_enc_write_va(2, SP_CMD_SET_EXT_CLOCK_EN, enable)) RES_RETURN(r_EDEVICE, -1);

  uint8_t reply;
  if (ser_enc_read_va(1, &reply)) return -1;

  if (reply != SP_SIG_OK) RES_RETURN(r_EDEVICE, -1);

  ext_clk_en = enable;

  RES_RETURN(r_ENONE, 0);
}

bool dev_get_ext_clk_en(void) {
  return ext_clk_en;
}

int dev_set_ext_clk(bool is_high) {
  if (!dev_is_ready()) RES_RETURN(r_ENOT_CONNECTED, -1);

  if (ser_enc_write_va(2, SP_CMD_SET_EXT_CLOCK, is_high)) RES_RETURN(r_EDEVICE, -1);

  uint8_t reply;
  if (ser_enc_read_va(1, &reply)) return -1;

  if (reply != SP_SIG_OK) RES_RETURN(r_EDEVICE, -1);

  ext_clk = is_high;

  RES_RETURN(r_ENONE, 0);
}

bool dev_get_ext_clk(void) {
  return ext_clk;
}

int dev_step_ext_clk(uint8_t counts) {
  if (counts == 0) RES_RETURN(r_EBOUNDS, -1);

  if (!dev_is_ready()) RES_RETURN(r_ENOT_CONNECTED, -1);

  if (ser_enc_write_va(2, SP_CMD_STEP_EXT_CLK, counts)) RES_RETURN(r_EDEVICE, -1);

  uint8_t reply;
  if (ser_enc_read_va(1, &reply)) return -1;

  if (reply != SP_SIG_OK) RES_RETURN(r_EDEVICE, -1);

  ext_clk = HIGH;

  RES_RETURN(r_ENONE, 0);
}

int dev_set_cpu_en(bool enable) {
  if (!dev_is_ready()) RES_RETURN(r_ENOT_CONNECTED, -1);

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
  if (!dev_is_ready()) RES_RETURN(r_ENOT_CONNECTED, -1);
  if (state == NULL) RES_RETURN(r_ENULL_PTR, -1);

  if (ser_enc_write_va(1, SP_CMD_GET_CPU_STATE)) RES_RETURN(r_EDEVICE, -1);

  uint8_t return_code;
  if (ser_enc_read_va(2, &return_code, state)) return -1;

  if (return_code != SP_SIG_OK) RES_RETURN(r_EDEVICE, -1);

  RES_RETURN(r_ENONE, 0);
}

int dev_print_bus_state(void) {
  if (!dev_is_ready()) RES_RETURN(r_ENOT_CONNECTED, -1);

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
  if (!dev_is_ready()) RES_RETURN(r_ENOT_CONNECTED, -1);
  if (data == NULL) RES_RETURN(r_ENULL_PTR, -1);
  
  if (ser_enc_write_va(1, SP_CMD_READ_DATABUS)) RES_RETURN(r_EDEVICE, -1);

  uint8_t return_code;
  if (ser_enc_read_va(2, &return_code, data)) return -1;

  if (return_code != SP_SIG_OK) RES_RETURN(r_EDEVICE, -1);

  RES_RETURN(r_ENONE, 0);
}

int dev_read_addrbus(uint16_t* addr) {
  if (!dev_is_ready()) RES_RETURN(r_ENOT_CONNECTED, -1);
  if (addr == NULL) RES_RETURN(r_ENULL_PTR, -1);
  
  if (ser_enc_write_va(1, SP_CMD_READ_ADDRBUS)) return -1;

  uint8_t return_code;
  uint8_t hb, lb;

  if (ser_enc_read_va(3, &return_code, &lb, &hb)) return -1;

  if (return_code != SP_SIG_OK) RES_RETURN(r_EDEVICE, -1);

  *addr = (hb << 8) | lb;

  RES_RETURN(r_ENONE, 0);
}

int dev_set_reset(bool reset) {
  if (!dev_is_ready()) RES_RETURN(r_ENOT_CONNECTED, -1);

  uint8_t return_code;
  if (ser_enc_write_va(2, SP_CMD_SET_RESET, reset)) return -1;
  if (ser_enc_read_va(1, &return_code)) return -1;
  if (return_code != SP_SIG_OK) RES_RETURN(r_EDEVICE, -1);

  reset_cpu = reset;

  RES_RETURN(r_ENONE, 0);
}

bool dev_get_reset(void) {
  return reset_cpu;
}

int dev_reset_cpu(void) {
  if (!dev_is_ready()) RES_RETURN(r_ENOT_CONNECTED, -1);

  if (dev_set_reset(true)) return -1;
  if (dev_set_reset(false)) return -1;

  RES_RETURN(r_ENONE, 0);
}

int dev_revert_state(void) {
  if (dev_set_ext_clk_en(ext_clk_en)) return -1;
  if (dev_set_ext_clk(ext_clk)) return -1;
  if (dev_set_reset(reset_cpu)) return -1;
  if (dev_set_cpu_en(cpu_en)) return -1;

  RES_RETURN(r_ENONE, 0);
}

int dev_step_instructions(uint8_t instructions) {
  if (instructions == 0) RES_RETURN(r_EBOUNDS, -1);
  if (!dev_is_ready()) RES_RETURN(r_ENOT_CONNECTED, -1);

  if (ser_enc_write_va(2, SP_CMD_STEP_INST_EXT_CLK, instructions)) return -1;

  uint8_t return_code;
  uint8_t executed_instructions;
  if (ser_enc_read_va(2, &return_code, &executed_instructions)) return -1;

  if (return_code != SP_SIG_OK) {
    RES_RETURN(r_EDEVICE, executed_instructions);
  }

  ext_clk = HIGH;

  RES_RETURN(r_ENONE, executed_instructions);
}

int dev_mem_read(uint16_t addr, uint8_t* data) {
  if (!dev_is_ready()) RES_RETURN(r_ENOT_CONNECTED, -1);
  if (data == NULL) RES_RETURN(r_ENULL_PTR, -1);

  uint8_t hb = addr >> 8;
  uint8_t lb = addr & 0xFF;

  if (ser_enc_write_va(3, SP_CMD_MEM_READ, lb, hb)) RES_RETURN(r_EDEVICE, -1);

  uint8_t return_code;
  if (ser_enc_read_va(2, &return_code, data)) return -1;

  if (return_code != SP_SIG_OK) RES_RETURN(r_EDEVICE, -1);
  
  if (dev_revert_state()) return -1;

  RES_RETURN(r_ENONE, 0);
}

int dev_mem_write(uint16_t addr, uint8_t data) {
  if (!dev_is_ready()) RES_RETURN(r_ENOT_CONNECTED, -1);

  uint8_t hb = addr >> 8;
  uint8_t lb = addr & 0xFF;

  if (ser_enc_write_va(4, SP_CMD_MEM_WRITE, lb, hb, data)) return -1;

  uint8_t return_code;
  if (ser_enc_read_va(1, &return_code)) return -1;

  if (return_code != SP_SIG_OK) RES_RETURN(r_EDEVICE, -1);

  if (dev_revert_state()) return -1;

  RES_RETURN(r_ENONE, 0);
}

int dev_mem_page_read(uint16_t addr, uint8_t count, uint8_t* data) {
  if (!dev_is_ready()) RES_RETURN(r_ENOT_CONNECTED, -1);
  if (data == NULL) RES_RETURN(r_ENULL_PTR, -1);
  if (count == 0 || count > PAGE_SIZE) RES_RETURN(r_EBOUNDS, -1);

  uint8_t hb = addr >> 8;
  uint8_t lb = addr & 0xFF;

  if (ser_enc_write_va(4, SP_CMD_MEM_PAGE_READ, lb, hb, count)) RES_RETURN(r_EDEVICE, -1);

  uint8_t reply_len = count + 3;
  uint8_t reply[UCOBS_MAX_DATA_LEN];
  if (ser_enc_read(&reply_len, reply)) return -1;

  uint8_t return_code = reply[0];
  if (return_code != SP_SIG_OK) RES_RETURN(r_EDEVICE, -1);
  memcpy(data, reply + 1, count);

  if (dev_revert_state()) return -1;

  RES_RETURN(r_ENONE, 0);
}

int dev_mem_page_write(uint16_t addr, uint8_t count, const uint8_t* data) {
  if (!dev_is_ready()) RES_RETURN(r_ENOT_CONNECTED, -1);
  if (data == NULL) RES_RETURN(r_ENULL_PTR, -1);
  if (count == 0 || count > PAGE_SIZE) RES_RETURN(r_EBOUNDS, -1);

  uint8_t hb = addr >> 8;
  uint8_t lb = addr & 0xFF;

  uint8_t command[4 + PAGE_SIZE] = {SP_CMD_MEM_PAGE_WRITE, lb, hb, count};

  memcpy(command + 4, data, count);

  if (ser_enc_write(count + 4, command)) return -1;

  uint8_t reply;
  if (ser_enc_read_va(1, &reply)) return -1;

  if (reply != SP_SIG_OK) RES_RETURN(r_EDEVICE, -1);

  if (dev_revert_state()) return -1;

  RES_RETURN(r_ENONE, 0);
}

