#include "bus_interface.h"
#include "io.h"
#include "io_defs.h"
#include "serial.h"
#include "twi.h"
#include "ucobs.h"
#include "utils.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <util/delay.h>
#include <stdbool.h>


#define LED_PIN           PB5
#define SET_LED(STATE)    PORTB = (PORTB & ~_BV(LED_PIN)) | ((STATE) << LED_PIN)
#define TOGGLE_LED()      PORTB = PORTB ^ _BV(LED_PIN)
#define LED_PERIOD        500

uint8_t recv_buf[UCOBS_MAX_PACKET_LEN];
uint16_t recv_index = 0;
bool await_sync = true;

int main(void) {
  DDRB |= _BV(LED_PIN);
  
  twi_init();
  ser_init();

  sei();

  if (bif_set_dev_en(true)) reset();
  if (bif_set_ext_clk_en(true)) reset();
  if (bif_set_cpu_en(true)) reset();
  if (bif_set_ext_clk(io_HIGH)) reset();

  while (true) {
    uint8_t data;
    uint16_t addr;

    if (bif_read_databus(&data)) reset();
    if (bif_read_addrbus(&addr)) reset();

    bool writing;
    if (bif_read_rw(&writing)) reset();

    bool bus_owned;
    if (bif_read_dev_tbo(&bus_owned)) reset();

    bool sync;
    if (bif_read_sync(&sync)) reset();

    ser_printf("CPU is %s 0x%04x: 0x%02x\n", writing ? "WRITING" : "READING", addr, data);
    ser_printf("Bus is %sowned, CPU is %sfetching opcode\n", bus_owned ? "" : "not ", sync ? "" : "not ");

    while (ser_read(NULL));

    if (bif_set_ext_clk(io_LOW)) reset();
    if (bif_set_ext_clk(io_HIGH)) reset();
  }

  /*
    if (ser_read(recv_buf + recv_index)) continue;
    if (recv_index >= UCOBS_MAX_PACKET_LEN) {
      recv_index = 0;
      await_sync = true;
    }
    
    if (recv_buf[recv_index] == 0x00) {
      if (await_sync) await_sync = false;
      else {
        int len = 0;
        if (recv_index) len = ucobs_decode(recv_index, recv_buf, recv_buf);
        recv_index = 0;
      }
    } else {
      recv_index++;
    }
  }
  */

  reset();
}

