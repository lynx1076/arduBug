#include "commands.h"
#include "result.h"
#include "serial_protocol.h"
#include "ucobs.h"
#include "utils.h"
#include "vector.h"
#include "serial.h"
#include <asm-generic/errno.h>
#include <stddef.h>
#include <ncurses.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>


static Vec device_list = VEC_EMPTY;
static ssize_t cycle = 0;

int cmd_help(size_t arg_cnt, const char **tokens) {
  (void)tokens;

  if (arg_cnt != 0) {
    RES_RETURN(r_EARGS, -1);
  }

#define X(FUNCTION, CMD, ALT) \
  printw("%s (%s) => %s\n", CMD, ALT, #FUNCTION);
  COMMANDS
#undef X

  RES_RETURN(r_ENONE, 0);
}

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
    printw("Connected to %s\n", ser_get_device());
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

  printw("Pinging %s\n", ser_get_device());

  if (ser_enc_write_va(1, SP_CMD_PING)) return -1;

  uint8_t reply;
  if (ser_enc_read_va(1, &reply)) return -1;
  
  printw("Ping successful\n");

  if (reply != SP_SIG_OK) {
    printw("Ping failed: %u", reply);
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

  printw("Getting version %s\n", ser_get_device());

  if (ser_enc_write_va(1, SP_CMD_VERSION_TEXT)) return -1;
  printw("Write ok\n");

  uint8_t reply[UCOBS_MAX_PACKET_LEN_NO_FRAME];
  size_t reply_len;
  if (ser_enc_read(&reply_len, reply)) return -1;
  if (_res != r_DATA_READY) RES_RETURN(r_ENO_DATA, -1);

  if (reply_len == 0) RES_RETURN(r_ENO_DATA, -1);

  if (reply[0] != SP_SIG_OK) RES_RETURN(r_EDEVICE, -1);
  
  printw("Version [%zu]: '%.*s'\n", reply_len - 1, (int)reply_len - 1, reply + 1);

  RES_RETURN(r_ENONE, 0);
}

int cmd_show_debug_logs(size_t arg_cnt, const char** tokens) {
  if (arg_cnt != 1) {
    RES_RETURN(r_EARGS, -1);
  }

  bool enable;
  if (parse_bool(tokens[1], &enable)) return -1;

  if (enable) {
    dbg_log_to_user = true;
    debug_log(log_INFO, "Enabled logging to stdout");
    printw("Enabled logging to stdout\n");
  } else {
    dbg_log_to_user = false;
    debug_log(log_INFO, "Disabled logging to stdout");
    printw("Disabled logging to stdout\n");
  }

  RES_RETURN(r_ENONE, 0);
}

int cmd_ext_clock_en(size_t arg_cnt, const char** tokens) {
  if (arg_cnt != 1) RES_RETURN(r_EARGS, -1);

  if (!ser_is_open()) {
    RES_RETURN(r_EDEVICE, -1);
  }

  bool enable;
  if (parse_bool(tokens[1], &enable)) return -1;

  printw("Successfully %s the external clock\n", enable ? "enabled" : "disabled");

  RES_RETURN(r_ENONE, 0);
}

