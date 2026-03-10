#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include "serial.h"
#include "serial_handler.h"
#include "twi.h"
#include "mcp23017.h"


#define LED_PIN           PB5
#define SET_LED(STATE)    PORTB = (PORTB & ~_BV(PB5)) | ((STATE) << PB5)

#define BAUD_RATE         57600

int main(void) {
  twi_init();
  ser_init(BAUD_RATE);
  sei();

  DDRB |= _BV(LED_PIN);

  bool led_state = false;

  while (mcp_check_health()) {
    SET_LED(led_state);
    led_state = !led_state;
    _delay_ms(100);
  }

  SET_LED(1);

  while (1) {
    seh_handle_command();
  }
}

