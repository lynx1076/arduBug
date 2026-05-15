#include "debugger.h"
#include "io.h"
#include "pins.h"
#include "result.h"
#include "utils.h"
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <termios.h>

result pulse_clock(const size_t n) {
  result _res;

  debug_log(log_INFO, "Starting to pulse clock %zu time%s", n == 1 ? "" : "s", n);

  bool base_state;
  _res = io_get_output_pin_state(PINS_CTRL_PIN_EXT_CLK, &base_state);
  if (_res != r_ENONE) return _res;

  _res = io_write_pin(PINS_CTRL_PIN_EXT_CLK, HIGH);
  if (_res != r_ENONE) return _res;

  for (size_t i = 0; i < n; i++) {
    _res = io_write_pin(PINS_CTRL_PIN_EXT_CLK, LOW);
    if (_res != r_ENONE) return _res;
    _res = io_write_pin(PINS_CTRL_PIN_EXT_CLK, HIGH);
    if (_res != r_ENONE) return _res;
  }

  debug_log(log_INFO, "Finished clock pulsing");

  return r_ENONE;
}

result reset_seq(void) {
  result _res;

  _res = io_write_pin(PINS_CTRL_PIN_EXT_RESETB, LOW);
  if (_res != r_ENONE) return _res;

  _res = pulse_clock(2);
  if (_res != r_ENONE) return _res;

  _res = io_write_pin(PINS_CTRL_PIN_EXT_RESETB, HIGH);
  if (_res != r_ENONE) return _res;

  _res = pulse_clock(7);
  if (_res != r_ENONE) return _res;

  return r_ENONE;
}

result read_databus(uint8_t* databus) {
  result _res;

  *databus = 0;

  for (int i = 0; i < 8; i++) {
    bool state;
    
    _res = io_read_pin(PINS_DATA_PIN_0 + i, &state);
    if (_res != r_ENONE) return _res;

    *databus |= (state ? 1 : 0) << i;
  }

  return r_ENONE;
}

result read_addrbus(uint16_t* addrbus) {
  result _res;

  *addrbus = 0;

  for (int i = 0; i < 16; i++) {
    bool state;
    
    _res = io_read_pin(PINS_ADDR_PIN_0 + i, &state);
    if (_res != r_ENONE) return _res;

    *addrbus |= (state ? 1 : 0) << i;
  }

  return r_ENONE;
}

result debugger(void) {
  result _res;

  while (true) {
    uint8_t databus;
    uint16_t addrbus;

    _res = read_databus(&databus);
    if (_res != r_ENONE) return _res;
    
    _res = read_addrbus(&addrbus);
    if (_res != r_ENONE) return _res;
    
    printf("[dbg] Read from 0x%04x => 0x%02x\n", addrbus, databus);

    getchar();
    pulse_clock(1);
  }
}

result dbg_start(const char** tokens, unsigned int arg_cnt) {
  (void)tokens;

  if (arg_cnt != 0) return r_EARGS;

  result _res;
  
  for (int i = 0; i < 32; i++) {
    _res = io_write_iodir(i, INPUT);
    if (_res != r_ENONE) return _res;
  }

  printf("A\n");
  _res = io_write_pin(PINS_CTRL_PIN_EXT_CLK_EN, HIGH);
  if (_res != r_ENONE) return _res;

  printf("B\n");
  _res = io_write_pin(PINS_CTRL_PIN_EXT_CLK, LOW);
  if (_res != r_ENONE) return _res;

  printf("C\n");
  _res = reset_seq();
  if (_res != r_ENONE) return _res;

  printf("D\n");
  _res = debugger();
  if (_res != r_ENONE) return _res;
  
  return r_ENONE;
}

