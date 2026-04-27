#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <util/delay.h>
#include <stdbool.h>
#include "ucobs.h"
#include "cmd_handler.h"
#include "serial.h"
#include "twi.h"
#include "ucobs.h"


#define LED_PIN           PB5
#define SET_LED(STATE)    PORTB = (PORTB & ~_BV(PB5)) | ((STATE) << PB5)
#define TOGGLE_LED()      PORTB = PORTB ^ _BV(PB5)
#define LED_PERIOD        500

#define RECV_BUFF_LEN     257

static uint32_t ms_counter = 0;
static uint32_t led_timer = 0;

void main_loop(void) {
  static uint8_t data_buff[RECV_BUFF_LEN];
  static int16_t data_buff_index = 0;
  static bool recv_sync = false;

  uint8_t byte;
  if (ser_read(&byte)) return;

  if (!recv_sync) {
    if (byte == 0x00) recv_sync = true;
    return;
  }

  if (byte != 0x00) {
    data_buff[data_buff_index++] = byte;
  } else {
    if ((data_buff_index = ucobs_decode(data_buff_index, data_buff, data_buff)) != -1) {
      uint8_t resp_len = cmd_exec(data_buff_index, data_buff);
      resp_len = ucobs_encode(resp_len, cmd_response_buf, data_buff);
      ser_write(0x00);
      ser_write_buf(resp_len, data_buff);
      ser_write(0x00);
    }
    data_buff_index = 0;
  }
}

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
    main_loop();
  }
}

ISR(TIMER0_COMPA_vect) {
  ms_counter++;

  if (ms_counter >= led_timer) {
    led_timer = ms_counter + LED_PERIOD / 2;
    TOGGLE_LED();
  }
}

