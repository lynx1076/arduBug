#include "io.h"
#include "pins.h"
#include "result.h"
#include "utils.h"
#include "ucobs.h"
#include "serial_protocol.h"
#include "serial.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>

static uint8_t reply[UCOBS_MAX_PACKET_LEN];
static size_t reply_length;

result io_get_pin_from_token(const char* token, uint8_t* pin) {
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

result io_get_state_from_token(const char* token, bool* state) {
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

result io_write_iodir(uint8_t pin, bool state) {
  result _res;

  _res = ser_enc_write_va(3, SP_CMD_WRITE_IODIR, pin, state);
  if (_res != r_ENONE) return _res;

  _res = ser_enc_read(&reply_length, reply);
  if (_res != r_DATA_READY) return _res;
  if (reply_length != 1) return r_EDEVICE;
  if (*reply != state) return r_EDEVICE;

  _res = BOOL_CMP(state, *reply) ? r_ENONE : r_EDEVICE;

  if (_res == r_ENONE) debug_log(log_INFO, "Successfully wrote %s to %u", state ? "INPUT" : "OUTPUT", pin);
  else {
    debug_log(log_INFO, "Failed to write %s to pin %u", state ? "INPUT" : "OUTPUT", pin);
    return _res;
  }

  return r_ENONE;
}

result io_write_pin(uint8_t pin, bool state) {
  result _res;

  _res = io_write_iodir(pin, OUTPUT);
  if (_res != r_ENONE) return _res;

  _res = ser_enc_write_va(3, SP_CMD_WRITE, pin, state);
  if (_res != r_ENONE) return _res;

  _res = ser_enc_read(&reply_length, reply);
  if (_res != r_DATA_READY) return _res;
  if (reply_length != 1) return r_EDEVICE;

  _res = BOOL_CMP(state, *reply) ? r_ENONE : r_EDEVICE;

  if (_res == r_ENONE) debug_log(log_INFO, "Successfully wrote %s to %u", state ? "HIGH" : "LOW", pin);
  else debug_log(log_INFO, "Failed to write %s to pin %u", state ? "HIGH" : "LOW", pin);

  return _res;
}

result io_get_output_pin_state(uint8_t pin, bool* state) {
  result _res;
  
  _res = ser_enc_write_va(2, SP_CMD_WRITE, pin);
  if (_res != r_ENONE) return _res;

  _res = ser_enc_read_va(1, reply);
  if (_res != r_DATA_READY) return _res;

  *state = *reply;

  return r_ENONE;
}

result io_read_pin(uint8_t pin, bool* state) {
  result _res;
  
  _res = io_write_iodir(pin, INPUT);
  if (_res != r_ENONE) return _res;

  _res = ser_enc_write_va(2, SP_CMD_READ, pin);
  if (_res != r_ENONE) return _res;

  _res = ser_enc_read_va(1, reply);
  if (_res != r_ENONE) return _res;

  *state = *reply;

  debug_log(log_INFO, "Read %s on pin %u", *state ? "HIGH" : "LOW", pin);

  return r_ENONE;
}

