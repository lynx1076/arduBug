#include "io.h"
#include "pins.h"
#include "result.h"
#include "utils.h"
#include "serial_protocol.h"
#include "serial.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>

typedef struct {
  int8_t is_high;
  int8_t io_mode;
} PinState;

static PinState pin_states[PIN_COUNT];

int io_init(void) {
  for (int i = 0; i < PIN_COUNT; i++) {
    pin_states[i] = (PinState){-1, -1};
  }

  RES_RETURN(r_ENONE, 0);
}

int io_get_pin_from_token(const char* token, uint8_t* pin) {
#define PIN_CASE(NAME, ALT, PIN) \
  if (strcmp(NAME, token) == 0 || strcmp(ALT, token) == 0) RES_RETURN(r_ENONE, PIN)
  
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
  if (parse_long(token, &pin_long)) return -1;

  if (pin_long < 0 || pin_long > 32) {
    RES_RETURN(r_EBOUNDS, -1);
  }

  *pin = (uint8_t)pin_long;

  RES_RETURN(r_ENONE, 0);
}

int io_get_state_from_token(const char* token, bool* state) {
#define STATE_CASE(NAME, STATE) \
  if (strcmp(NAME, token) == 0) do { *state = STATE; RES_RETURN(r_ENONE, 0); } while (0)
  
  STATE_CASE("1", HIGH);
  STATE_CASE("high", HIGH);

  STATE_CASE("0", LOW);
  STATE_CASE("low", LOW);

  STATE_CASE("output", OUTPUT);
  STATE_CASE("out", OUTPUT);
  STATE_CASE("in", INPUT);

#undef STATE_CASE

  RES_RETURN(r_EPARSE, -1);
}

bool io_pin_writable(int pin) {
  switch (pin) {
    case PINS_CTRL_PIN_RWB: return false;
    case PINS_CTRL_PIN_NMIB: return false;
    case PINS_CTRL_PIN_IRQB: return false;
    case PINS_CTRL_PIN_VPB: return false;
    case PINS_CTRL_PIN_EXT_CLK_EN: return true;
    case PINS_CTRL_PIN_EXT_CLK: return true;
    case PINS_CTRL_PIN_EXT_RESETB: return true;
    case PINS_CTRL_PIN_EXT_RWB: return false;

    default: true;
  }
}

int io_write_iodir(uint8_t pin, bool set_input) {
  result _res;

  if (pin_states[pin].io_mode != -1) {
    if (BOOL_CMP(pin_states[pin].io_mode, set_input)) {
      debug_log(log_INFO, "Skipped writing %s to %u as pin is already configured", set_input ? "INPUT" : "OUTPUT", pin);
      RES_RETURN(r_ENONE, 0);
    }
  }

  if (ser_enc_write_va(3, SP_CMD_WRITE_IODIR, pin, set_input ? INPUT : OUTPUT)) return -1;

  bool io_mode_read;
  if (ser_enc_read_va(1, &io_mode_read)) return -1;


  if (BOOL_CMP(set_input, io_mode_read)) {
    debug_log(log_INFO, "Successfully wrote %s to %u", set_input ? "INPUT" : "OUTPUT", pin);
  } else {
    debug_log(log_INFO, "Failed to write %s to pin %u", set_input ? "INPUT" : "OUTPUT", pin);
    RES_RETURN(r_EDEVICE, -1);
  }

  pin_states[pin].io_mode = set_input ? INPUT : OUTPUT;
  pin_states[pin].is_high = -1;

  RES_RETURN(r_ENONE, 0);
}

int io_write_pin(uint8_t pin, bool set_high) {
  if (io_write_iodir(pin, OUTPUT)) return -1;

  if (pin_states[pin].is_high != -1) {
    if (BOOL_CMP(pin_states[pin].is_high, set_high)) {
      debug_log(log_INFO, "Skipped writing %s to pin %u as pin is already configured", set_high ? "HIGH" : "LOW", pin);
      RES_RETURN(r_ENONE, 0);
    }
  }

  if (ser_enc_write_va(3, SP_CMD_WRITE, pin, set_high)) return -1;

  bool is_high_read;
  if (ser_enc_read_va(1, &is_high_read)) return -1;

  if (BOOL_CMP(is_high_read, set_high)) {
    debug_log(log_INFO, "Successfully wrote %s to %u", set_high ? "HIGH" : "LOW", pin);
  } else {
    debug_log(log_INFO, "Failed to write %s to pin %u", set_high ? "HIGH" : "LOW", pin);
    RES_RETURN(r_EDEVICE, -1);
  }

  pin_states[pin].is_high = set_high;

  RES_RETURN(r_ENONE, 0);
}

int io_read_pin(uint8_t pin, bool* is_high) {
  if (io_write_iodir(pin, INPUT)) return -1;

  if (ser_enc_write_va(2, SP_CMD_READ, pin)) return -1;

  if (ser_enc_read_va(1, is_high)) return -1;

  debug_log(log_INFO, "Read from pin %u -> %s", pin, *is_high ? "HIGH" : "LOW");

  RES_RETURN(r_ENONE, 0);
}

int io_set_clock(bool phase) {
  if (io_write_pin(PINS_CTRL_PIN_EXT_CLK, phase ? HIGH : LOW)) return -1;

  RES_RETURN(r_ENONE, 0);
}

int io_pulse_clock(size_t n) {
  debug_log(log_INFO, "Starting to pulse clock %zu time%s", n, n == 1 ? "" : "s");

  bool clock_state;
  if (io_read_pin(PINS_CTRL_PIN_EXT_CLK, &clock_state)) return -1;

  if (BOOL_CMP(clock_state, HIGH) && n != 0) {
    n--;
    if (io_set_clock(LOW)) return -1;
  }

  for (size_t i = 0; i < n; i++) {
    if (io_set_clock(HIGH)) return -1;
    if (io_set_clock(LOW)) return -1;
  }

  debug_log(log_INFO, "Finished clock pulsing");

  RES_RETURN(r_ENONE, 0);
}

int io_reset_seq(void) {
  if (io_write_pin(PINS_CTRL_PIN_EXT_RESETB, LOW)) return -1;

  if (io_pulse_clock(2)) return -1;

  if (io_write_pin(PINS_CTRL_PIN_EXT_RESETB, HIGH)) return -1;

  if (io_pulse_clock(7)) return -1;

  RES_RETURN(r_ENONE, 0);
}

int io_read_databus(uint8_t* databus) {
  *databus = 0;

  for (int i = 0; i < 8; i++) {
    bool state;
    
    if (io_read_pin(PINS_DATA_PIN_0 + i, &state)) return -1;

    *databus |= (state ? 1 : 0) << i;
  }

  RES_RETURN(r_ENONE, 0);
}

int io_read_addrbus(uint16_t* addrbus) {
  *addrbus = 0;

  for (int i = 0; i < 16; i++) {
    bool state;
    
    if (io_read_pin(PINS_ADDR_PIN_0 + i, &state)) return -1;

    *addrbus |= (state ? 1 : 0) << i;
  }

  RES_RETURN(r_ENONE, 0);
}

int io_write_databus(uint8_t databus) {
  for (int i = 0; i < 8; i++) {
    bool state = databus & (1 << i);
    
    if (io_write_pin(PINS_DATA_PIN_0 + i, state)) return -1;
  }

  RES_RETURN(r_ENONE, 0);
}

