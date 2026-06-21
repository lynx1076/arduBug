#include "serial.h"
#include "twi.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>


#define LED_PIN           PB5
#define SET_LED(STATE)    PORTB = (PORTB & ~_BV(PB5)) | ((STATE) << PB5)
#define TOGGLE_LED()      PORTB = PORTB ^ _BV(PB5)
#define LED_PERIOD        500

#define RECV_BUFF_LEN     257

int main(void) {
  DDRB |= _BV(LED_PIN);
  
  twi_init();
  ser_init();

  sei();

  ser_printf("Scanning...\n");
  for (int i = 0x08; i < 128; i++) {
    if (!twi_check_device_present(i)) {
      ser_printf("Device found at 0x%02x\n", i);
    }
  }
  ser_printf("Scan done\n");

  while (true);
}

