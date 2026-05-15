#include "commands.h"
#include "io.h"
#include "pins.h"
#include "result.h"
#include "serial_protocol.h"
#include "ucobs.h"
#include "utils.h"
#include "vector.h"
#include "serial.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Vec device_list = VEC_ZERO;

result cmd_quit(const char** tokens, unsigned int arg_cnt) {
  (void)tokens;

  if (arg_cnt != 0) {
    return r_EARGS;
  }

  printf("Releasing resources:\n");

  char* device_path = ser_get_current_port();
  if (device_path) {
    printf("-> Closing port: %s\n", device_path);
    ser_close();
  } else {
    printf("-> No port to close\n");
  }
  printf("All resources released. Quitting...\n");
  
  exit(0);

  return r_ENONE;
}

result cmd_scan_devices(const char** tokens, unsigned int arg_cnt) {
  (void)tokens;

  result _res;
  if (arg_cnt != 0) {
    return r_EARGS;
  }
  
  if (device_list.memory) vec_free(&device_list);

  _res = ser_scan_ports(&device_list);
  if (_res != r_ENONE) return _res;

  printf("Found %zu open ports:\n", device_list.count);
  for (size_t i = 0; i < device_list.count; i++) {
    printf("\t[%zu]: %s\n", i, (char*)vec_get(&device_list, i));
  }

  return r_ENONE;
}

result cmd_connect(const char** tokens, unsigned int arg_cnt) {
  (void)tokens;
  
  debug_log(log_INFO, "Attempting to connect");

  result _res;
  if (arg_cnt != 1) {
    return r_EARGS;
  }
  
  long device_index;
  _res = parse_long(tokens[1], &device_index);
  if (_res != r_ENONE) return _res;

  char* device_path = vec_get(&device_list, device_index);

  if (device_path == NULL) {
    printf("Device at index does not exist\n");
    return r_ECMD;
  }

  printf("Attempting to connect to [%li]: %s\n", device_index, device_path);

  _res = ser_open(device_path);
  if (_res != r_ENONE) {
    printf("Failed to connect to port\n");
    return _res;
  }

  printf("Opened port %s successfuly\n", device_path);

  return r_ENONE;
}

result cmd_disconnect(const char** tokens, unsigned int arg_cnt) {
  (void)tokens;

  if (arg_cnt != 0) {
    return r_EARGS;
  }

  if (ser_is_open()) {
    printf("Closed serial port\n");
    ser_close();
  } else {
    printf("No port to close\n");
  }

  return r_ENONE;
}

result cmd_status(const char** tokens, unsigned int arg_cnt) {
  (void)tokens;

  if (arg_cnt != 0) {
    return r_EARGS;
  }

  if (ser_is_open()) {
    printf("Connected to %s\n", ser_get_current_port());
  } else {
    printf("Not connected\n");
  }

  return r_ENONE;
}

result cmd_ping(const char** tokens, unsigned int arg_cnt) {
  (void)tokens;

  result _res;

  if (arg_cnt != 0) {
    return r_EARGS;
  }

  if (!ser_is_open()) {
    return r_EDEVICE;
  }

  printf("Pinging %s\n", ser_get_current_port());

  _res = ser_enc_write_va(1, SP_CMD_PING);
  if (_res != r_ENONE) return _res;

  uint8_t reply;
  _res = ser_enc_read_va(1, &reply);
  if (_res != r_ENONE) return _res;
  
  printf("Ping successful\n");

  debug_log(log_INFO, "Read device compat code as %u", reply);

  if (reply != SP_COMPAT_CODE) {
    printf("Warning: serial protocol compat codes are not equal\n");
    printf("Software compat code: %u", SP_COMPAT_CODE);
    printf("Device compat code: %u", reply);
  }

  return r_ENONE;
}

result cmd_version(const char** tokens, unsigned int arg_cnt) {
  (void)tokens;

  result _res;

  if (arg_cnt != 0) {
    return r_EARGS;
  }

  if (!ser_is_open()) {
    return r_EDEVICE;
  }

  printf("Getting version %s\n", ser_get_current_port());

  _res = ser_enc_write_va(1, SP_CMD_VERSION_TEXT);
  if (_res != r_ENONE) return _res;

  uint8_t reply[UCOBS_MAX_PACKET_LEN_NO_FRAME];
  size_t reply_len;
  _res = ser_enc_read(&reply_len, reply);
  if (_res != r_DATA_READY) return _res;
  
  if (reply_len > 0) {
    printf("Version [%zu]: '%.*s'\n", reply_len, (int)reply_len, reply);
  } else {
    printf("Received empty packet\n");
    return r_EDEVICE;
  }

  return r_ENONE;
}

result cmd_show_debug_logs(const char** tokens, unsigned int arg_cnt) {
  if (arg_cnt != 1) {
    return r_EARGS;
  }

  if (strcmp(tokens[1], "true") == 0) {
    dbg_log_to_stdout = true;
    debug_log(log_INFO, "Enabled logging to stdout");
    printf("Enabled logging to stdout\n");
  } else if (strcmp(tokens[1], "false") == 0) {
    dbg_log_to_stdout = false;
    debug_log(log_INFO, "Disabled logging to stdout");
    printf("Disabled logging to stdout\n");
  } else {
    return r_EPARSE;
  }

  return r_ENONE;
}

