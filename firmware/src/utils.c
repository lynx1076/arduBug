#include "utils.h"
#include <util/delay.h>
#include <avr/io.h>
#include <stdbool.h>

void(*_reset)(void) = 0;

void setup_led(void) {
  DDRB |= _BV(LED_PIN);
}

void reset(void) {
  _delay_ms(200);
  _reset();
}

void panic(void) {
  SET_LED(0);

  while (true) {
    BLINK_LED(80);
    _delay_ms(80);
    BLINK_LED(80);
    _delay_ms(80);
    BLINK_LED(80);
    _delay_ms(400);
  }
}

