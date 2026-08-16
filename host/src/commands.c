#include "commands.h"
#include "common.h"
#include "device.h"
#include "device.h"
#include "devmem.h"
#include "result.h"
#include "serial_protocol.h"
#include "ucobs.h"
#include "utils.h"
#include "parse.h"
#include "vector.h"
#include "serial.h"
#include "gui.h"
#include "parse.h"
#include <raylib.h>
#include <stddef.h>
#include <ncurses.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>


#define TOKEN_COUNT       256

static Vec device_list = VEC_EMPTY;

static int tokenize(char* str, char** tokens) {
  char* new_token = strtok(str, " \t");
  int token_cnt = 0;
  
  while (new_token != NULL) {
    tokens[token_cnt] = new_token;
    token_cnt++;

    new_token = strtok(NULL, " \t");
  }

  return token_cnt;
}

int cmd_execute(char* cmd) {
  char* input_tokens[TOKEN_COUNT];
  int token_cnt = tokenize(cmd, input_tokens);

	if (token_cnt == 0) RES_RETURN(r_ENONE, 0);
#define X(CMD_FUNC, CMD, ALT) \
	else if (strcmp(CMD, input_tokens[0]) == 0 || strcmp(ALT ? ALT : "", input_tokens[0]) == 0) { \
    debug_log(log_INFO, "Running command: %s", #CMD_FUNC); \
	  return CMD_FUNC(token_cnt - 1, (const char**)input_tokens); \
  }

	COMMANDS
#undef X
  
  RES_RETURN(r_EINVALID_CMD, -1);
}

int cmd_help(size_t arg_cnt, const char **tokens) {
  (void)tokens;

  if (arg_cnt != 0) {
    RES_RETURN(r_EARGS, -1);
  }

#define X(FUNCTION, CMD, ALT) \
  gui_log(TextFormat("%s (%s) => %s\n", CMD, ALT ? ALT : "", #FUNCTION));
  COMMANDS
#undef X

  RES_RETURN(r_ENONE, 0);
}

int cmd_quit(size_t arg_cnt, const char** tokens) {
  (void)tokens;

  if (arg_cnt != 0) {
    RES_RETURN(r_EARGS, -1);
  }

  program_should_close = true;

  RES_RETURN(r_ESYS, -1);
}

int cmd_scan_devices(size_t arg_cnt, const char** tokens) {
  (void)tokens;

  if (arg_cnt != 0) {
    RES_RETURN(r_EARGS, -1);
  }
  
  if (device_list.memory) vec_free(&device_list);

  gui_log(TextFormat("Scanning..."));

  if (ser_scan_ports(&device_list)) return -1;

  gui_log(TextFormat("Found %zu open port%s:\n", device_list.count, device_list.count == 1 ? "" : "s"));
  for (size_t i = 0; i < device_list.count; i++) {
    gui_log(TextFormat("\t[%zu]: %s\n", i, (char*)vec_get(&device_list, i)));
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
    gui_log(TextFormat("Device at index does not exist\n"));
    RES_RETURN(r_EARGS, -1);
  }

  debug_log(log_INFO, "Attempting to connect to [%li]: %s", device_index, device_path);
  gui_log(TextFormat("Attempting to connect to [%li]: %s\n", device_index, device_path));

  if (ser_open(device_path)) return -1;

  gui_log(TextFormat("Opened port %s successfuly\n", device_path));

  RES_RETURN(r_ENONE, 0);
}

int cmd_disconnect(size_t arg_cnt, const char** tokens) {
  (void)tokens;

  if (arg_cnt != 0) {
    RES_RETURN(r_EARGS, -1);
  }

  if (ser_is_open()) {
    gui_log(TextFormat("Closed serial port\n"));
    ser_close();
  } else {
    gui_log(TextFormat("No port to close\n"));
  }

  RES_RETURN(r_ENONE, 0);
}

int cmd_status(size_t arg_cnt, const char** tokens) {
  (void)tokens;

  if (arg_cnt != 0) {
    RES_RETURN(r_EARGS, -1);
  }

  if (ser_is_open()) {
    gui_log(TextFormat("Connected to %s", ser_get_device()));
    if (dev_is_ready()) {
      gui_log("Device ready");
    }
  } else {
    gui_log(TextFormat("Not connected\n"));
  }

  RES_RETURN(r_ENONE, 0);
}

int cmd_ping(size_t arg_cnt, const char** tokens) {
  (void)tokens;
  (void)arg_cnt;

  gui_log(TextFormat("Pinging %s\n", ser_get_device()));

  if (dev_ping()) return -1;

  gui_log("Success");

  RES_RETURN(r_ENONE, 0);
}

int cmd_compat_code(size_t arg_cnt, const char** tokens) {
  (void)arg_cnt;
  (void)tokens;

  uint8_t dev_compat_code;
  if (dev_get_compat_code(&dev_compat_code)) return -1;


  if (dev_compat_code != SP_COMPAT_CODE) {
    gui_log("Compat code mismatch!");
    gui_log(TextFormat("Host: 0x%02x != device: 0x%02x", SP_COMPAT_CODE, dev_compat_code));
    ser_close();
    RES_RETURN(r_ECOMPATIBILITY, -1);
  } else {
    gui_log(TextFormat("Host: 0x%02x == device: 0x%02x", SP_COMPAT_CODE, dev_compat_code));
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

  gui_log(TextFormat("Getting version %s\n", ser_get_device()));

  if (ser_enc_write_va(1, SP_CMD_VERSION_TEXT)) return -1;

  uint8_t reply[UCOBS_MAX_PACKET_LEN_NO_FRAME];
  uint8_t reply_len = UCOBS_MAX_DATA_LEN;
  if (ser_enc_read(&reply_len, reply)) return -1;
  if (_res != r_DATA_READY) RES_RETURN(r_ENO_DATA, -1);

  if (reply_len == 0) RES_RETURN(r_ENO_DATA, -1);

  if (reply[0] != SP_SIG_OK) RES_RETURN(r_EDEVICE, -1);
  
  gui_log(TextFormat("Version [%zu]: '%.*s'\n", reply_len - 1, (int)reply_len - 1, reply + 1));

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
    gui_log(TextFormat("Enabled logging to stdout\n"));
  } else {
    dbg_log_to_user = false;
    debug_log(log_INFO, "Disabled logging to stdout");
    gui_log(TextFormat("Disabled logging to stdout\n"));
  }

  RES_RETURN(r_ENONE, 0);
}

int cmd_clear_gui_logs(size_t arg_cnt, const char** tokens) {
  (void)arg_cnt;
  (void)tokens;

  gui_log_clear();

  RES_RETURN(r_ENONE, 0);
}

int cmd_ext_clock_en(size_t arg_cnt, const char** tokens) {
  if (arg_cnt != 1) RES_RETURN(r_EARGS, -1);

  bool enable;
  if (parse_bool(tokens[1], &enable)) return -1;

  if (dev_set_ext_clk_en(enable)) {
    gui_log("Failed to set the external clock enable\n");
    return -1;
  } else {
    gui_log(TextFormat("Successfully %s the external clock\n", enable ? "enabled" : "disabled"));
  }

  RES_RETURN(r_ENONE, 0);
}

int cmd_read_bus_state(size_t arg_cnt, const char** tokens) {
  (void)arg_cnt;
  (void)tokens;

  if (dev_print_bus_state()) return -1;

  RES_RETURN(r_ENONE, 0);
}

int cmd_step_clock(size_t arg_cnt, const char** tokens) {
  const long MAX_STEPS = 200;
  long steps;

  if (arg_cnt == 0) steps = 1;
  else if (arg_cnt == 1) {
    if (parse_long(tokens[1], &steps)) return -1;
    if (steps > MAX_STEPS) {
      gui_log(TextFormat("Too many steps, only up to %li steps at a time", MAX_STEPS));
      RES_RETURN(r_EARGS, -1);
    } else if (steps <= 0) {
      gui_log("Must step clock atleast once");
      RES_RETURN(r_EARGS, -1);
    }
  } else {
    RES_RETURN(r_EARGS, -1);
  }

  if (dev_step_ext_clk(steps)) return -1;

  if (dev_print_bus_state()) return -1;

  RES_RETURN(r_ENONE, 0);
}

int cmd_reset_cpu(size_t arg_cnt, const char** tokens) {
  (void)arg_cnt;
  (void)tokens;
  
  if (dev_reset_cpu()) return -1;

  gui_log("Successfully reset CPU");

  RES_RETURN(r_ENONE, 0);
}

int cmd_step_instruction(size_t arg_cnt, const char** tokens) {
  const long MAX_STEPS = 255;
  long _steps;

  if (arg_cnt == 0) _steps = 1;
  else if (arg_cnt == 1) {
    if (parse_long(tokens[1], &_steps)) return -1;
    if (_steps > MAX_STEPS) {
      gui_log(TextFormat("Too many steps, only up to %li steps at a time", MAX_STEPS));
      RES_RETURN(r_EARGS, -1);
    } else if (_steps <= 0) {
      gui_log("Must step clock atleast once");
      RES_RETURN(r_EARGS, -1);
    }
  } else {
    RES_RETURN(r_EARGS, -1);
  }

  int steps = _steps;
  int executed_steps = dev_step_instructions(steps);

  if (_res == r_ENONE) {
    gui_log(TextFormat("Stepped %i instructions", executed_steps));
  } else {
    gui_log("Command failed");
    gui_log(TextFormat("Stepped %i instrcutions instead of %i", executed_steps, steps));
  }

  if (dev_print_bus_state()) return -1;

  RES_RETURN(r_ENONE, 0);
}

int cmd_mem_read(size_t arg_cnt, const char** tokens) {
  long count = 1;
  if (arg_cnt != 1) {
    if (arg_cnt == 2) {
      count = -1;
    } else {
      RES_RETURN(r_EARGS, -1);
    }
  }

  uint16_t addr;
  if (parse_hex_word(tokens[1], &addr)) return -1;

  if (count < 0) {
    if (parse_long(tokens[2], &count)) return -1;
  }

  if (count == 0 || count > PAGE_SIZE) RES_RETURN(r_EBOUNDS, -1);

  uint8_t data[PAGE_SIZE];
  if (count == 1) {
    if (dev_mem_read(addr, data)) return -1;
  } else {
    if (dev_mem_bulk_read(addr, count, data)) return -1;
  }

  for (int i = 0; i < count; i++) {
    gui_log(TextFormat("0x%04x => 0x%02x", addr + i, data[i]));
  }

  RES_RETURN(r_ENONE, 0);
}

int cmd_mem_write(size_t arg_cnt, const char** tokens) {
  if (arg_cnt < 2) RES_RETURN(r_EARGS, -1);

  long count = arg_cnt - 1;
  if (count == 0 || count > PAGE_SIZE) RES_RETURN(r_EBOUNDS, -1);

  uint16_t addr;
  if (parse_hex_word(tokens[1], &addr)) return -1;

  uint8_t data[PAGE_SIZE];

  for (size_t i = 0; i < arg_cnt - 1; i++) {
    if (parse_hex_byte(tokens[i + 2], data + i)) return -1;
  }

  if (count == 1) {
    if (dev_mem_write(addr, *data)) return -1;
  } else {
    if (dev_mem_bulk_write(addr, count, data)) return -1;
  }

  gui_log("Write okay");

  RES_RETURN(r_ENONE, 0);
}

int cmd_mem_dump(size_t arg_cnt, const char** tokens) {
  (void)arg_cnt;
  (void)tokens;

  const char* filepath = "dump.txt";

  if (arg_cnt > 0) filepath = tokens[1];

  if (devm_dump_to_file(filepath)) return -1;

  RES_RETURN(r_ENONE, 0);
}

int cmd_mem_flash(size_t arg_cnt, const char** tokens) {
  (void)arg_cnt;
  (void)tokens;

  const char* filepath = "flash.txt";

  if (arg_cnt > 0) filepath = tokens[1];

  if (devm_flash_from_file(filepath)) return -1;

  RES_RETURN(r_ENONE, 0);
}

