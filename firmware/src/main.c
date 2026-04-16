#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <util/delay.h>
#include <stdbool.h>
#include "cmd_handler.h"
#include "serial_handler.h"
#include "cmd_handler.h"
#include "serial.h"
#include "twi.h"


#define LED_PIN           PB5
#define SET_LED(STATE)    PORTB = (PORTB & ~_BV(PB5)) | ((STATE) << PB5)
#define TOGGLE_LED()      PORTB = PORTB ^ _BV(PB5)
#define LED_PERIOD        500


uint32_t ms_counter = 0;
uint32_t led_timer = 0;

int main(void) {
  TCCR0A = (1 << WGM01);
  TCCR0B = (1 << CS01) | (1 << CS00);
  OCR0A = 249;
  TIMSK0 = (1 << OCIE0A);

  DDRB |= _BV(LED_PIN);
  
  twi_init();
  ser_init();

  sei();

  while (true) {
    seh_receive(&cmd_exec);
    if (ms_counter >= led_timer) {
      led_timer = ms_counter + LED_PERIOD / 2;
      TOGGLE_LED();
    }
  }
}

ISR(TIMER0_COMPA_vect) {
  ms_counter++;
}

