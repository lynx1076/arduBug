#include "cmd_handler.h"
#include "bus_interface.h"
#include "io.h"
#include "serial_protocol.h"
#include "meta.h"
#include "common.h"
#include <stdint.h>
#include <string.h>

uint8_t cmd_exec(uint8_t len, const uint8_t* cmd, uint8_t* resp) {
  const uint8_t* args = cmd + 1;
  uint8_t arg_cnt = len - 1;

  switch (*cmd) {
    case SP_CMD_GET_CPU_STATE: {
      uint8_t state = bif_get_cpu_state();
      if (state & (1 << SP_STATE_ERROR)) break;

      resp[0] = SP_SIG_OK;
      resp[1] = state;

      return 2;
    }
    case SP_CMD_SET_CPU_EN: {
      if (arg_cnt != 1) break;
      if (io_set_cpu_en(args[0])) break;
      
      resp[0] = SP_SIG_OK;
      return 1;
    }
    case SP_CMD_SET_EXT_CLOCK_EN: {
      if (arg_cnt != 1) break;
      if (io_set_ext_clk_en(args[0])) break;

      resp[0] = SP_SIG_OK;
      return 1;
    }
    case SP_CMD_SET_EXT_CLOCK: {
      if (arg_cnt != 1) break;
      io_set_ext_clk(args[0] ? HIGH : LOW);

      resp[0] = SP_SIG_OK;
      return 1;
    }
    case SP_CMD_READ_DATABUS: {
      resp[0] = SP_SIG_OK;
      resp[1] = io_read_databus();
      return 2;
    }
    case SP_CMD_READ_ADDRBUS: {
      uint16_t addrbus;
      if (io_read_addrbus(&addrbus)) break;
      uint8_t lb = addrbus & 0xFF;
      uint8_t hb = addrbus >> 8;
      resp[0] = SP_SIG_OK;
      resp[1] = lb;
      resp[2] = hb;
      return 3;
    }
    case SP_CMD_MEM_READ: {
      if (arg_cnt != 2) break;

      uint8_t lb = args[0];
      uint8_t hb = args[1];
      uint16_t addr = hb << 8 | lb;
      uint8_t data;

      if (bif_mem_read(addr, &data)) break;

      resp[0] = SP_SIG_OK;
      resp[1] = data;

      return 2;
    }
    case SP_CMD_MEM_WRITE: {
      if (arg_cnt != 3) break;

      uint8_t lb = args[0];
      uint8_t hb = args[1];
      uint16_t addr = hb << 8 | lb;
      uint8_t data = args[2];

      if (bif_mem_write(addr, data)) break;

      resp[0] = SP_SIG_OK;
      return 1;
    }
    case SP_CMD_MEM_PAGE_READ: {
      if (arg_cnt != 3) break;

      uint8_t lb = args[0];
      uint8_t hb = args[1];
      uint16_t addr = hb << 8 | lb;
      uint8_t cnt = args[2];
      if (cnt == 0 || cnt > PAGE_SIZE) break;
      uint8_t data[PAGE_SIZE];

      if (bif_mem_page_read(addr, cnt, data)) break;

      resp[0] = SP_SIG_OK;
      for (uint8_t i = 0; i < cnt; i++) {
        resp[i + 1] = data[i];
      }

      return 1 + cnt;
    }
    case SP_CMD_MEM_PAGE_WRITE: {
      if (arg_cnt < 4) break; // Addr LB & HB + Count + minimum 1 Byte

      uint8_t lb = args[0];
      uint8_t hb = args[1];
      uint16_t addr = hb << 8 | lb;
      uint8_t cnt = args[2];

      if (cnt != arg_cnt - 3) break; // Addr LB & HB + Count

      if (bif_mem_page_write(addr, cnt, args + 3)) break;

      resp[0] = SP_SIG_OK;

      return 1;
    }
    case SP_CMD_SET_RESET: {
      if (arg_cnt != 1) break;
      if (io_set_reset(args[0])) break;

      resp[0] = SP_SIG_OK;
      return 1;
    }
    case SP_CMD_STEP_EXT_CLK: {
      uint8_t cnt;

      if (arg_cnt == 0) cnt = 1;
      else if (arg_cnt == 1) cnt = args[1];
      else break;

      for (uint8_t i = 0; i < cnt; i++) {
        io_set_ext_clk(LOW);
        io_set_ext_clk(HIGH);
      }

      resp[0] = SP_SIG_OK;
      resp[1] = cnt;

      return 1;
    }
    case SP_CMD_STEP_INST_EXT_CLK: {
      uint8_t cnt;

      if (arg_cnt == 0) cnt = 1;
      else if (arg_cnt == 1) cnt = args[1];
      else break;

      uint8_t executed_steps = 0;

      for (uint8_t i = 0; i < cnt; i++) {
        if (bif_step_instruction()) break;
        executed_steps++;
      }

      resp[0] = executed_steps == cnt ? SP_SIG_OK : SP_SIG_ERR;
      resp[1] = executed_steps;

      return 2;
    }
    case SP_CMD_COMPAT_CODE: {
      resp[0] = SP_SIG_OK;
      resp[1] = SP_COMPAT_CODE;
      return 2;
    }
    case SP_CMD_VERSION_TEXT: {
      resp[0] = SP_SIG_OK;
      strncpy((char*)resp + 1, VERSION_TXT, 254);
      return strlen(VERSION_TXT) + 1;
    }
    case SP_CMD_PING: {
      resp[0] = SP_SIG_OK;
      return 1;
    }
    default: break;
  }

  resp[0] = SP_SIG_ERR;
  return 1;
}

