#include "bus_interface.h"
#include "io.h"
#include "meta.h"
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


uint8_t recv_buf[UCOBS_MAX_PACKET_LEN];
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

  ser_printf("Init %s\n\n", VERSION_TXT);
  
  const uint8_t test[] = {
    0x78,
    0xD8,
    0xA2, 0xFF,
    0x9A,
    0xEE, 0x00, 0x02,
    0x4C, 0x05, 0x80
  };

  ser_printf("Flashing program\n");
  for (uint8_t i = 0; i < sizeof(test) / sizeof(*test); i++) {
    uint16_t addr = 0x8000 + i;
    if (bif_mem_write(addr, test[i])) reset();
    ser_printf("Prog 0x%02x to 0x%04x\n", test[i], addr);
  }
  ser_printf("Finished flashing\n");

  ser_printf("Prog reset vec\n");
  if (bif_mem_write(0xfffc, 0x00)) reset();
  if (bif_mem_write(0xfffd, 0x80)) reset();
  ser_printf("Done prog reset vec\n");

  if (bif_set_dev_en(true)) reset();
  if (bif_set_ext_clk_en(true)) reset();
  if (bif_set_ext_clk(iol_HIGH)) reset();
  if (bif_set_cpu_en(true)) reset();
  if (io_flush()) return 1;

  ser_printf("Entering main loop\n");

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

    bool vp;
    if (bif_read_vp(&vp)) reset();

    if (vp) ser_printf("Cpu is fetching vector\n");
    if (sync) ser_printf("Cpu is fetching opcode\n");
    if (!bus_owned) ser_printf("Bus is not owned\n");
    ser_printf("CPU is %s 0x%04x: 0x%02x\n\n", writing ? "WRITING" : "READING", addr, data);

    if (bif_set_ext_clk(iol_LOW)) reset();
    if (bif_set_ext_clk(iol_HIGH)) reset();

    while (ser_read(NULL));
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

