#include "commands.h"
#include "cli.h"
#include "result.h"
#include "serial_protocol.h"
#include "ucobs.h"
#include "utils.h"
#include "vector.h"
#include "serial.h"
#include "io.h"
#include "pins.h"
#include <stddef.h>
#include <ncurses.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>


#define FAIL_TO_NORMAL \
  do { printw("Error: switching back to normal mode\n"); cmd_init_normal(0, NULL); return -1; } while (0)

static Vec device_list = VEC_EMPTY;
static ssize_t cycle = 0;
static uint8_t ext_data = 0;

int cmd_quit(size_t arg_cnt, const char** tokens) {
  (void)tokens;

  if (arg_cnt != 0) {
    RES_RETURN(r_EARGS, -1);
  }

  quit(0);

  RES_RETURN(r_ESYS, -1);
}

int cmd_scan_devices(size_t arg_cnt, const char** tokens) {
  (void)tokens;

  if (arg_cnt != 0) {
    RES_RETURN(r_EARGS, -1);
  }
  
  if (device_list.memory) vec_free(&device_list);

  printw("Scanning...");

  if (ser_scan_ports(&device_list)) return -1;

  printw("Found %zu open port%s:\n", device_list.count, device_list.count == 1 ? "" : "s");
  for (size_t i = 0; i < device_list.count; i++) {
    printw("\t[%zu]: %s\n", i, (char*)vec_get(&device_list, i));
  }

  RES_RETURN(r_ENONE, 0);
}

int cmd_connect(size_t arg_cnt, const char** tokens) {
  (void)tokens;
  
  if (arg_cnt != 1) {
    RES_RETURN(r_EARGS, -1);
  }
  
  long device_index;
  if (parse_long(tokens[1], &device_index)) return -1;

  char* device_path = vec_get(&device_list, device_index);

  if (device_path == NULL) {
    printw("Device at index does not exist\n");
    RES_RETURN(r_ECMD, -1);
  }

  debug_log(log_INFO, "Attempting to connect to [%li]: %s\n", device_index, device_path);
  printw("Attempting to connect to [%li]: %s\n", device_index, device_path);

  if (ser_open(device_path)) return -1;

  printw("Opened port %s successfuly\n", device_path);

  RES_RETURN(r_ENONE, 0);
}

int cmd_disconnect(size_t arg_cnt, const char** tokens) {
  (void)tokens;

  if (arg_cnt != 0) {
    RES_RETURN(r_EARGS, -1);
  }

  if (ser_is_open()) {
    printw("Closed serial port\n");
    ser_close();
  } else {
    printw("No port to close\n");
  }

  RES_RETURN(r_ENONE, 0);
}

int cmd_status(size_t arg_cnt, const char** tokens) {
  (void)tokens;

  if (arg_cnt != 0) {
    RES_RETURN(r_EARGS, -1);
  }

  if (ser_is_open()) {
    printw("Connected to %s\n", ser_get_current_port());
  } else {
    printw("Not connected\n");
  }

  RES_RETURN(r_ENONE, 0);
}

int cmd_ping(size_t arg_cnt, const char** tokens) {
  (void)tokens;

  if (arg_cnt != 0) {
    RES_RETURN(r_EARGS, -1);
  }

  if (!ser_is_open()) {
    RES_RETURN(r_EDEVICE, -1);
  }

  printw("Pinging %s\n", ser_get_current_port());

  if (ser_enc_write_va(1, SP_CMD_PING)) return -1;

  uint8_t reply;
  if (ser_enc_read_va(1, &reply)) return -1;
  
  printw("Ping successful\n");

  debug_log(log_INFO, "Read device compat code as %u", reply);

  if (reply != SP_COMPAT_CODE) {
    printw("Warning: serial protocol compat codes are not equal\n");
    printw("Software compat code: %u", SP_COMPAT_CODE);
    printw("Device compat code: %u", reply);
  }

  RES_RETURN(r_ENONE, 0);
}

int cmd_version(size_t arg_cnt, const char** tokens) {
  (void)tokens;

  if (arg_cnt != 0) {
    RES_RETURN(r_EARGS, -1);
  }

  if (!ser_is_open()) {
    RES_RETURN(r_EDEVICE, -1);
  }

  printw("Getting version %s\n", ser_get_current_port());

  if (ser_enc_write_va(1, SP_CMD_VERSION_TEXT)) return -1;
  printw("Write ok\n");

  uint8_t reply[UCOBS_MAX_PACKET_LEN_NO_FRAME];
  size_t reply_len;
  if (ser_enc_read(&reply_len, reply)) return -1;
  if (_res != r_DATA_READY) RES_RETURN(r_ENO_DATA, -1);
  
  if (reply_len > 0) {
    printw("Version [%zu]: '%.*s'\n", reply_len, (int)reply_len, reply);
  } else {
    printw("Received empty packet\n");
    RES_RETURN(r_ENO_DATA, -1);
  }

  RES_RETURN(r_ENONE, 0);
}

int cmd_show_debug_logs(size_t arg_cnt, const char** tokens) {
  if (arg_cnt != 1) {
    RES_RETURN(r_EARGS, -1);
  }

  if (strcmp(tokens[1], "true") == 0) {
    dbg_log_to_user = true;
    debug_log(log_INFO, "Enabled logging to stdout");
    printw("Enabled logging to stdout\n");
  } else if (strcmp(tokens[1], "false") == 0) {
    dbg_log_to_user = false;
    debug_log(log_INFO, "Disabled logging to stdout");
    printw("Disabled logging to stdout\n");
  } else {
    RES_RETURN(r_EPARSE, -1);
  }

  RES_RETURN(r_ENONE, 0);
}

int cmd_init_normal(size_t arg_cnt, const char **tokens) {
  (void)tokens;

  if (arg_cnt != 0) {
    RES_RETURN(r_EARGS, -1);
  }

  cliState = CliState_Normal;
  if (io_init()) return -1;

  RES_RETURN(r_ENONE, 0);
}

int cmd_init_debug(size_t arg_cnt, const char** tokens) {
  (void)tokens;

  if (arg_cnt != 0) {
    RES_RETURN(r_EARGS, -1);
  }

  cycle = 0;
  cliState = CliState_Debug;

  if (io_init()) FAIL_TO_NORMAL;
  
  if (io_write_pin(PINS_CTRL_PIN_EXT_CLK_EN, HIGH)) FAIL_TO_NORMAL;

  if (io_write_pin(PINS_CTRL_PIN_EXT_CLK, LOW)) FAIL_TO_NORMAL;

  debug_log(log_INFO, "Debugger initalized");
  printw("Debugger initalized\n");

  RES_RETURN(r_ENONE, 0);
}

int cmd_init_emulation(size_t arg_cnt, const char** tokens) {
  (void)tokens;

  if (arg_cnt != 0) {
    RES_RETURN(r_EARGS, -1);
  }

  cycle = 0;
  cliState = CliState_EmulateMemory;

  if (io_init()) FAIL_TO_NORMAL;
  
  if (io_write_pin(PINS_CTRL_PIN_EXT_CLK_EN, HIGH)) FAIL_TO_NORMAL;

  if (io_write_pin(PINS_CTRL_PIN_EXT_CLK, LOW)) FAIL_TO_NORMAL;

  debug_log(log_INFO, "Emulator initalized");
  printw("Emulator initalized\n");

  RES_RETURN(r_ENONE, 0);
}

int cmd_step(size_t arg_cnt, const char** tokens) {
  (void)tokens;

  if (cliState != CliState_Debug && cliState != CliState_EmulateMemory) {
    printw("Not in an expected mode\n");
    RES_RETURN(r_ENOT_INIT, -1);
  }

  if (arg_cnt != 0) {
    RES_RETURN(r_EARGS, -1);
  }

  if (io_set_clock(HIGH)) FAIL_TO_NORMAL;

  uint8_t databus;
  uint16_t addrbus;
  bool cpu_reading;

  if (io_cpu_reading(&cpu_reading)) FAIL_TO_NORMAL;
  if (io_read_addrbus(&addrbus)) FAIL_TO_NORMAL;

  if (cliState == CliState_EmulateMemory && cpu_reading) {
    if (io_write_databus(ext_data)) FAIL_TO_NORMAL;
    databus = ext_data;
  } else {
    if (io_read_databus(&databus)) FAIL_TO_NORMAL;
  }

  printw("\n");
  printw("Cycle: %zi\n", cycle);

  printw("CPU %s 0x%02x @ 0x%04x\n", cpu_reading ? "reading" : "writing", databus, addrbus);

  if (io_set_clock(LOW)) FAIL_TO_NORMAL;

  cycle++;

  return 0;
}

int cmd_reset(size_t arg_cnt, const char **tokens) {
  (void)tokens;

  if (cliState != CliState_Debug && cliState != CliState_EmulateMemory) {
    printw("Not in debug mode\n");
    RES_RETURN(r_ENOT_INIT, -1);
  }

  if (arg_cnt != 0) {
    RES_RETURN(r_EARGS, -1);
  }

  printw("Resetting...\n");

  if (io_reset_seq()) FAIL_TO_NORMAL;

  cycle = -8;

  RES_RETURN(r_ENONE, 0);
}

int cmd_set_data(size_t arg_cnt, const char **tokens) {
  if (arg_cnt != 1) RES_RETURN(r_EARGS, -1);

  if (parse_hex_byte(tokens[1], &ext_data)) return -1;

  RES_RETURN(r_ENONE, 0);
}

