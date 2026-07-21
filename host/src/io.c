#include "io.h"
#include "serial.h"
#include "serial_protocol.h"
#include "result.h"
#include <stdbool.h>
#include <stdint.h>

bool ext_clk_en = true;
bool ext_clk = false;

int io_init(void) {
  if (io_set_ext_clk_en(true)) return -1;
  if (io_set_ext_clk(LOW)) return -1;

  RES_RETURN(r_ENONE, -1);
}

int io_set_ext_clk_en(bool enable) {
  if (ser_enc_write_va(2, SP_CMD_SET_EXT_CLOCK_EN, enable)) RES_RETURN(r_EDEVICE, -1);

  uint8_t reply;
  if (ser_enc_read_va(1, &reply)) return -1;

  if (reply != SP_SIG_OK) RES_RETURN(r_EDEVICE, -1);

  ext_clk_en = enable;

  RES_RETURN(r_ENONE, 0);
}

int io_set_ext_clk(bool high) {
  if (ser_enc_write_va(2, SP_CMD_SET_EXT_CLOCK, high)) RES_RETURN(r_EDEVICE, -1);

  uint8_t reply;
  if (ser_enc_read_va(1, &reply)) return -1;

  if (reply != SP_SIG_OK) RES_RETURN(r_EDEVICE, -1);

  ext_clk = high;

  RES_RETURN(r_ENONE, 0);
}

int io_get_cpu_state(uint8_t* state) {
  if (ser_enc_write_va(1, SP_CMD_GET_CPU_STATE)) RES_RETURN(r_EDEVICE, -1);

  uint8_t return_code;
  if (ser_enc_read_va(2, &return_code, state)) {}

  if (return_code != SP_SIG_OK) RES_RETURN(r_EDEVICE, -1);

  RES_RETURN(r_ENONE, 0);
}

