#include "io.h"
#include "cli.h"
#include "pins.h"
#include "result.h"
#include "utils.h"
#include "serial_protocol.h"
#include "serial.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

typedef struct {
  int8_t state;
  int8_t io_mode;
} PinState;

static PinState pin_states[PIN_COUNT];

int io_init(void) {
  for (int i = 0; i < PIN_COUNT; i++) {
    pin_states[i] = (PinState){-1, -1};
  }

  if (io_write_iodir(PINS_CTRL_PIN_EXT_RWB, INPUT)) return -1;
  if (io_write_pin(PINS_CTRL_PIN_EXT_CLK_EN, HIGH)) return -1;
  if (io_write_pin(PINS_CTRL_PIN_EXT_RESETB, HIGH)) return -1;

  for (int i = 0; i < 8; i++) {
    if (io_write_iodir(PINS_DATA_PIN_0 + i, INPUT)) return -1;
  }

  for (int i = 0; i < 16; i++) {
    if (io_write_iodir(PINS_ADDR_PIN_0 + i, INPUT)) return -1;
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

  PIN_CASE("ADDR8", "a8", PINS_ADDR_PIN_8);
  PIN_CASE("ADDR9", "a9", PINS_ADDR_PIN_9);
  PIN_CASE("ADDR10", "a10", PINS_ADDR_PIN_10);
  PIN_CASE("ADDR11", "a11", PINS_ADDR_PIN_11);
  PIN_CASE("ADDR12", "a12", PINS_ADDR_PIN_12);
  PIN_CASE("ADDR13", "a13", PINS_ADDR_PIN_13);
  PIN_CASE("ADDR14", "a14", PINS_ADDR_PIN_14);
  PIN_CASE("ADDR15", "a15", PINS_ADDR_PIN_15);

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

int io_pin_writable(int pin, bool* writable) {
  *writable = false;

  switch (pin) {
    case PINS_CTRL_PIN_RWB:
    case PINS_CTRL_PIN_NMIB:
    case PINS_CTRL_PIN_IRQB:
    case PINS_CTRL_PIN_VPB: {
      *writable = false;
    } RES_RETURN(r_ENONE, 0);
    case PINS_CTRL_PIN_EXT_RWB:
    case PINS_CTRL_PIN_EXT_CLK_EN:
    case PINS_CTRL_PIN_EXT_CLK:
    case PINS_CTRL_PIN_EXT_RESETB: {
      *writable = true;
    } RES_RETURN(r_ENONE, 0);

    case PINS_DATA_PIN_0:
    case PINS_DATA_PIN_1:
    case PINS_DATA_PIN_2:
    case PINS_DATA_PIN_3:
    case PINS_DATA_PIN_4:
    case PINS_DATA_PIN_5:
    case PINS_DATA_PIN_6:
    case PINS_DATA_PIN_7: {
      if (cliState != CliState_Override && cliState != CliState_EmulateMemory) RES_RETURN(r_ENONE, 0);

      if (cliState == CliState_EmulateMemory) {
        *writable = true;
        RES_RETURN(r_ENONE, 0);
      }

      bool clock_phase;
      if (io_read_pin_output(PINS_CTRL_PIN_EXT_CLK, &clock_phase)) return -1;
      if (BOOL_CMP(clock_phase, LOW)) *writable = true;
    } RES_RETURN(r_ENONE, 0);

    case PINS_ADDR_PIN_0:
    case PINS_ADDR_PIN_1:
    case PINS_ADDR_PIN_2:
    case PINS_ADDR_PIN_3:
    case PINS_ADDR_PIN_4:
    case PINS_ADDR_PIN_5:
    case PINS_ADDR_PIN_6:
    case PINS_ADDR_PIN_7:
    case PINS_ADDR_PIN_8:
    case PINS_ADDR_PIN_9:
    case PINS_ADDR_PIN_10:
    case PINS_ADDR_PIN_11:
    case PINS_ADDR_PIN_12:
    case PINS_ADDR_PIN_13:
    case PINS_ADDR_PIN_14:
    case PINS_ADDR_PIN_15: {
      if (cliState != CliState_Override) RES_RETURN(r_ENONE, 0);

      bool clock_phase;
      if (io_read_pin_output(PINS_CTRL_PIN_EXT_CLK, &clock_phase)) return -1;
      if (BOOL_CMP(clock_phase, LOW)) *writable = true;
    } RES_RETURN(r_ENONE, 0);
    default: RES_RETURN(r_EARGS, -1);
  }
}

int io_write_iodir(uint8_t pin, bool iodir) {
  if (BOOL_CMP(iodir, OUTPUT)) {
    bool writable;
    if (io_pin_writable(pin, &writable)) return -1;

    if (!writable) RES_RETURN(r_EACCESS, -1);
  }

  if (pin_states[pin].io_mode != -1) {
    if (BOOL_CMP(pin_states[pin].io_mode, iodir)) {
      debug_log(log_INFO, "Skipped writing %s to %u as pin is already configured", iodir ? "INPUT" : "OUTPUT", pin);
      RES_RETURN(r_ENONE, 0);
    }
  }

  if (ser_enc_write_va(3, SP_CMD_WRITE_IODIR, pin, iodir)) return -1;

  bool io_mode_read;
  if (ser_enc_read_va(1, &io_mode_read)) return -1;


  if (BOOL_CMP(iodir, io_mode_read)) {
    debug_log(log_INFO, "Successfully wrote %s to %u", iodir ? "INPUT" : "OUTPUT", pin);
  } else {
    debug_log(log_INFO, "Failed to write %s to pin %u", iodir ? "INPUT" : "OUTPUT", pin);
    RES_RETURN(r_EDEVICE, -1);
  }

  pin_states[pin].io_mode = iodir;
  pin_states[pin].state = -1;

  RES_RETURN(r_ENONE, 0);
}

int io_write_pin(uint8_t pin, bool state) {
  if (io_write_iodir(pin, OUTPUT)) return -1;

  if (pin_states[pin].state != -1) {
    if (BOOL_CMP(pin_states[pin].state, state)) {
      debug_log(log_INFO, "Skipped writing %s to pin %u as pin is already configured", state ? "HIGH" : "LOW", pin);
      RES_RETURN(r_ENONE, 0);
    }
  }

  if (ser_enc_write_va(3, SP_CMD_WRITE, pin, state)) return -1;

  bool state_read;
  if (ser_enc_read_va(1, &state_read)) return -1;

  if (BOOL_CMP(state_read, state)) {
    debug_log(log_INFO, "Successfully wrote %s to %u", state ? "HIGH" : "LOW", pin);
  } else {
    debug_log(log_INFO, "Failed to write %s to pin %u", state ? "HIGH" : "LOW", pin);
    RES_RETURN(r_EDEVICE, -1);
  }

  pin_states[pin].state = state;

  RES_RETURN(r_ENONE, 0);
}

int io_read_pin(uint8_t pin, bool* state) {
  if (io_write_iodir(pin, INPUT)) return -1;

  if (ser_enc_write_va(2, SP_CMD_READ, pin)) return -1;
  if (ser_enc_read_va(1, state)) return -1;

  debug_log(log_INFO, "Read from pin %u -> %s", pin, *state ? "HIGH" : "LOW");

  RES_RETURN(r_ENONE, 0);
}

int io_read_pin_output(uint8_t pin, bool* state) {
  if (io_write_iodir(pin, OUTPUT)) return -1;

  if (pin_states[pin].state < 0) {
    if (ser_enc_write_va(2, SP_CMD_READ, pin)) return -1;
    if (ser_enc_read_va(1, state)) return -1;
    pin_states[pin].state = *state;
  } else {
    *state = pin_states[pin].state;
  }

  RES_RETURN(r_ENONE, 0);
}

int io_set_clock(bool phase) {
  if (io_write_pin(PINS_CTRL_PIN_EXT_CLK_EN, HIGH)) return -1;
  if (io_write_pin(PINS_CTRL_PIN_EXT_CLK, phase)) return -1;

  if (BOOL_CMP(phase, LOW)) {
    for (int i = 0; i < 8; i++) {
      if (io_write_iodir(PINS_DATA_PIN_0 + i, INPUT)) return -1;
    }
  }

  RES_RETURN(r_ENONE, 0);
}

int io_pulse_clock(size_t n) {
  debug_log(log_INFO, "Starting to pulse clock %zu time%s", n, n == 1 ? "" : "s");

  if (io_set_clock(LOW)) return -1;

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

  RES_RETURN(r_ENONE, 0);
}

int io_cpu_reading(bool* is_reading) {
  if (io_read_pin(PINS_CTRL_PIN_RWB, is_reading)) return -1;

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
    bool state = databus & (1 << i) ? HIGH : LOW;
    
    if (io_write_pin(PINS_DATA_PIN_0 + i, state)) return -1;
  }

  RES_RETURN(r_ENONE, 0);
}

int io_write_addrbus(uint16_t addrbus) {
  for (int i = 0; i < 16; i++) {
    bool state = addrbus & (1 << i) ? HIGH : LOW;
    
    if (io_write_pin(PINS_ADDR_PIN_0 + i, state)) return -1;
  }

  RES_RETURN(r_ENONE, 0);
}

