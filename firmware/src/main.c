#include "cmd_handler.h"
#include "io.h"
#include "serial.h"
#include "twi.h"
#include "ucobs.h"
#include "utils.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stddef.h>
#include <stdint.h>
#include <util/delay.h>
#include <stdbool.h>


uint8_t ucobs_buf[UCOBS_MAX_PACKET_LEN];
uint16_t recv_index = 0;
bool await_sync = true;

int main(void) {
  setup_led();
  
  twi_init();
  ser_init();

  sei();

  if (io_init()) {
    panic();
  }

  if (io_set_dev_en(true)) reset();
  if (io_set_ext_clk_en(true)) reset();
  io_set_ext_clk(LOW);
  if (io_set_cpu_en(true)) reset();

  while (true) {
    if (ser_read(ucobs_buf + recv_index)) continue;
    if (recv_index >= UCOBS_MAX_PACKET_LEN) {
      recv_index = 0;
      await_sync = true;
    }
    
    if (ucobs_buf[recv_index] == 0x00) {
      if (await_sync) await_sync = false;
      else {
        int len = 0;
        if (recv_index) len = ucobs_decode(recv_index, ucobs_buf, ucobs_buf);
        if (len) {
          len = cmd_exec(len, ucobs_buf, ucobs_buf);
          if (len <= 0) panic();
          len = ucobs_encode(len, ucobs_buf, ucobs_buf);
          
          ser_write(0x00);
          ser_write_buf(len, ucobs_buf);
          ser_write(0x00);
        }
        recv_index = 0;
      }
    } else {
      recv_index++;
    }
  }
}

