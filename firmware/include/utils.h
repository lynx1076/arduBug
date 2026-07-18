#ifndef UTILS_H
#define UTILS_H

#include <util/delay.h>

#define LED_PIN             PB5
#define SET_LED(STATE)      PORTB = (PORTB & ~_BV(LED_PIN)) | ((STATE) << LED_PIN)
#define TOGGLE_LED()        PORTB = PORTB ^ _BV(LED_PIN)
#define BLINK_LED(LENGTH)   do { TOGGLE_LED(); _delay_ms(LENGTH); TOGGLE_LED(); } while (0)

void setup_led(void);
void reset(void);
void panic(void);

#endif
