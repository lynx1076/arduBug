#include "commands.h"
#include "result.h"
#include "serial_protocol.h"
#include "ucobs.h"
#include "utils.h"
#include "vector.h"
#include "serial.h"
#include "pins.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Vec device_list = VEC_ZERO;


result get_pin_from_token(const char* token, uint8_t* pin) {
  result _res;

#define PIN_CASE(NAME, ALT, PIN) \
  if (strcmp(NAME, token) == 0 || strcmp(ALT, token) == 0) do { *pin= PIN; return r_ENONE; } while (0)
  
  PIN_CASE("RWB", "rw", PINS_CTRL_PIN_RWB);
  PIN_CASE("NMIB", "nmi", PINS_CTRL_PIN_NMIB);
  PIN_CASE("IRQB", "irq", PINS_CTRL_PIN_IRQB);
  PIN_CASE("VPB", "vp", PINS_CTRL_PIN_VPB);
  PIN_CASE("EXT_CLK_EN", "eclk_en", PINS_CTRL_PIN_EXT_CLK);
  PIN_CASE("EXT_CLK", "eclk", PINS_CTRL_PIN_EXT_CLK);
  PIN_CASE("EXT_RESETB", "eres", PINS_CTRL_PIN_EXT_RESETB);
  PIN_CASE("EXT_RWB", "erw", PINS_CTRL_PIN_EXT_RWB);

  PIN_CASE("DATA0", "d0", PINS_DATA_PIN_0);
  PIN_CASE("DATA1", "d1", PINS_DATA_PIN_1);
  PIN_CASE("DATA2", "d2", PINS_DATA_PIN_2);
  PIN_CASE("DATA3", "d3", PINS_DATA_PIN_3);
  PIN_CASE("DATA4", "d4", PINS_DATA_PIN_4);
  PIN_CASE("DATA5", "d5", PINS_DATA_PIN_5);
  PIN_CASE("DATA6", "d6", PINS_DATA_PIN_6);
  PIN_CASE("DATA7", "d7", PINS_DATA_PIN_7);
  
  PIN_CASE("ADDR0", "a0", PINS_ADDR_PIN_0);
  PIN_CASE("ADDR1", "a1", PINS_ADDR_PIN_1);
  PIN_CASE("ADDR2", "a2", PINS_ADDR_PIN_2);
  PIN_CASE("ADDR3", "a3", PINS_ADDR_PIN_3);
  PIN_CASE("ADDR4", "a4", PINS_ADDR_PIN_4);
  PIN_CASE("ADDR5", "a5", PINS_ADDR_PIN_5);
  PIN_CASE("ADDR6", "a6", PINS_ADDR_PIN_6);
  PIN_CASE("ADDR7", "a7", PINS_ADDR_PIN_7);

  PIN_CASE("ADDR8", "a8", PINS_ADDR_PIN_0);
  PIN_CASE("ADDR9", "a9", PINS_ADDR_PIN_1);
  PIN_CASE("ADDR10", "a10", PINS_ADDR_PIN_2);
  PIN_CASE("ADDR11", "a11", PINS_ADDR_PIN_3);
  PIN_CASE("ADDR12", "a12", PINS_ADDR_PIN_4);
  PIN_CASE("ADDR13", "a13", PINS_ADDR_PIN_5);
  PIN_CASE("ADDR14", "a14", PINS_ADDR_PIN_6);
  PIN_CASE("ADDR15", "a15", PINS_ADDR_PIN_7);

#undef PIN_CASE

  long pin_long;
  _res = parse_long(token, &pin_long);
  if (_res != r_ENONE) return _res;

  if (pin_long < 0 || pin_long > 32) {
    return r_EBOUNDS;
  }

  *pin = (uint8_t)pin_long;

  return r_ENONE;
}

result get_state_from_token(const char* token, bool* state) {
#define STATE_CASE(NAME, STATE) \
  if (strcmp(NAME, token) == 0) do { *state = STATE; return r_ENONE; } while (0)
  
  STATE_CASE("1", HIGH);
  STATE_CASE("high", HIGH);

  STATE_CASE("0", LOW);
  STATE_CASE("low", LOW);

  STATE_CASE("output", OUTPUT);
  STATE_CASE("out", OUTPUT);
  STATE_CASE("in", INPUT);

#undef STATE_CASE

  return r_EPARSE;
}

result cmd_quit(const char** tokens, unsigned int arg_cnt) {
  (void)tokens;

  if (arg_cnt != 0) {
    return r_ECMD;
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
    return r_ECMD;
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
    return r_ECMD;
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

  printf("Opened port %s successfully\n", device_path);

  return r_ENONE;
}

result cmd_disconnect(const char** tokens, unsigned int arg_cnt) {
  (void)tokens;

  if (arg_cnt != 0) {
    return r_ECMD;
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
    return r_ECMD;
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
    return r_ECMD;
  }

  if (!ser_is_open()) {
    return r_EDEVICE;
  }

  printf("Pinging %s\n", ser_get_current_port());

  _res = ser_enc_write_va(1, SP_CMD_PING);
  if (_res != r_ENONE) return _res;

  uint8_t reply[UCOBS_MAX_PACKET_LEN_NO_FRAME];
  int reply_len;
  _res = ser_enc_read(&reply_len, reply);
  if (_res != r_DATA_READY) return _res;
  
  if (reply_len != 1) {
    return r_EDEVICE;
  }

  printf("Ping successfull\n");

  if (*reply != SP_COMPAT_CODE) {
    printf("Warning: serial protocol compat codes are not equal\n");
    printf("Software compat code: %u", SP_COMPAT_CODE);
    printf("Device compat code: %u", *reply);
  }

  return r_ENONE;
}

result cmd_version(const char** tokens, unsigned int arg_cnt) {
  (void)tokens;

  result _res;

  if (arg_cnt != 0) {
    return r_ECMD;
  }

  if (!ser_is_open()) {
    return r_EDEVICE;
  }

  printf("Getting version %s\n", ser_get_current_port());

  _res = ser_enc_write_va(1, SP_CMD_VERSION_TEXT);
  if (_res != r_ENONE) return _res;

  uint8_t reply[UCOBS_MAX_PACKET_LEN_NO_FRAME];
  int reply_len;
  _res = ser_enc_read(&reply_len, reply);
  if (_res != r_DATA_READY) return _res;
  
  if (reply_len > 0) {
    printf("Version [%i]: '%.*s'\n", reply_len, reply_len, reply);
  } else {
    printf("Received empty packet\n");
    return r_EDEVICE;
  }

  return r_ENONE;
}

result cmd_show_debug_logs(const char** tokens, unsigned int arg_cnt) {
  if (arg_cnt != 1) {
    return r_ECMD;
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

result cmd_write(const char** tokens, unsigned int arg_cnt) {
  result _res;

  if (arg_cnt != 2) {
    return r_ECMD;
  }

  uint8_t pin;
  bool state;

  _res = get_pin_from_token(tokens[1], &pin);
  if (_res != r_ENONE) return _res;
  _res = get_state_from_token(tokens[2], &state);
  if (_res != r_ENONE) return _res;

  uint8_t reply[UCOBS_MAX_PACKET_LEN];
  int reply_length;

  _res = ser_enc_write_va(3, SP_CMD_WRITE_IODIR, pin, OUTPUT);
  if (_res != r_ENONE) return _res;

  _res = ser_enc_read(&reply_length, reply);
  if (_res != r_DATA_READY) return _res;
  if (reply_length != 1) return r_EDEVICE;
  if (*reply != OUTPUT) return r_EDEVICE;

  _res = ser_enc_write_va(3, SP_CMD_WRITE, pin, state);
  if (_res != r_ENONE) return _res;

  _res = ser_enc_read(&reply_length, reply);
  if (_res != r_DATA_READY) return _res;
  if (reply_length != 1) return r_EDEVICE;

  printf("Writing %s to %u: %s\n", state ? "HIGH" : "LOW", pin, !(*reply ^ state) ? "successfull" : "failed");
  printf("Pin %i is now %s\n", pin, *reply ? "HIGH" : "LOW");

  return r_ENONE;
}

result cmd_read(const char** tokens, unsigned int arg_cnt) {
  result _res;

  if (arg_cnt != 1) {
    return r_ECMD;
  }

  uint8_t pin;

  _res = get_pin_from_token(tokens[1], &pin);
  if (_res != r_ENONE) return _res;

  uint8_t reply[UCOBS_MAX_PACKET_LEN];
  int reply_length;

  _res = ser_enc_write_va(3, SP_CMD_WRITE_IODIR, pin, INPUT);
  if (_res != r_ENONE) return _res;

  _res = ser_enc_read(&reply_length, reply);
  if (_res != r_DATA_READY) return _res;
  if (reply_length != 1) return r_EDEVICE;
  if (*reply != INPUT) return r_EDEVICE;

  _res = ser_enc_write_va(2, SP_CMD_WRITE, pin);
  if (_res != r_ENONE) return _res;

  _res = ser_enc_read(&reply_length, reply);
  if (_res != r_DATA_READY) return _res;
  if (reply_length != 1) return r_EDEVICE;

  printf("Reading %u: %s\n", pin, *reply ? "HIGH" : "LOW");

  return r_ENONE;
}

result cmd_pulse(const char** tokens, unsigned int arg_cnt) {
  result _res;

  if (arg_cnt != 2) {
    return r_ECMD;
  }

  uint8_t pin;
  long delay_us;

  _res = get_pin_from_token(tokens[1], &pin);
  if (_res != r_ENONE) return _res;
  _res = parse_long(tokens[2], &delay_us);
  if (_res != r_ENONE) return _res;

  uint8_t reply[UCOBS_MAX_PACKET_LEN];
  int reply_length;

  _res = ser_enc_write_va(3, SP_CMD_WRITE_IODIR, pin, OUTPUT);
  if (_res != r_ENONE) return _res;

  _res = ser_enc_read(&reply_length, reply);
  if (_res != r_DATA_READY) return _res;
  if (reply_length != 1) return r_EDEVICE;
  if (*reply != OUTPUT) return r_EDEVICE;

  _res = ser_enc_write_va(2, SP_CMD_READ, pin);
  if (_res != r_ENONE) return _res;

  _res = ser_enc_read(&reply_length, reply);
  if (_res != r_DATA_READY) return _res;
  if (reply_length != 1) return r_EDEVICE;

  bool base_state = *reply;

  _res = ser_enc_write_va(3, SP_CMD_WRITE, pin, !base_state);
  if (_res != r_ENONE) return _res;

  _res = ser_enc_read(&reply_length, reply);
  if (_res != r_DATA_READY) return _res;
  if (reply_length != 1) return r_EDEVICE;

  printf("Writing %s to %u: %s\n", !base_state ? "HIGH" : "LOW", pin, !(*reply ^ !base_state) ? "successfull" : "failed");
  printf("Pin %i is now %s\n", pin, *reply ? "HIGH" : "LOW");

  printf("Sleeping for %ld\n", delay_us);
  sleep_us(delay_us);

  _res = ser_enc_write_va(3, SP_CMD_WRITE, pin, base_state);
  if (_res != r_ENONE) return _res;

  _res = ser_enc_read(&reply_length, reply);
  if (_res != r_DATA_READY) return _res;
  if (reply_length != 1) return r_EDEVICE;

  printf("Writing %s to %u: %s\n", base_state ? "HIGH" : "LOW", pin, !(*reply ^ base_state) ? "successfull" : "failed");
  printf("Pin %i is now %s\n", pin, *reply ? "HIGH" : "LOW");

  return r_ENONE;
}

