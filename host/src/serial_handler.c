#include "result.h"
#include "serial.h"
#include "serial_protocol.h"
#include "utils.h"
#include "pins.h"
#include <serial_handler.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define PACKET_BUFF_SIZE        254
#define PACKET_TIMEOUT_US       600000

bool recv_sync = false;
uint8_t packet_index = 0;
uint8_t block_len = 0;
uint8_t block_read = 0;
bool pending_zero = false;

uint8_t* packet_buff;

result seh_init(void) {
  packet_buff = malloc(PACKET_BUFF_SIZE);
  if (packet_buff == NULL) return r_EMEM;

  return r_ENONE;
}

void seh_free(void) {
  free(packet_buff);
}

void seh_update(void) {
  if (ser_just_opened()) {
    recv_sync = false;
  }
}

result seh_receive(uint8_t* len, uint8_t** data) {
  result res;
  uint8_t byte;

  uint64_t timer = micros() + PACKET_TIMEOUT_US;

  while (timer > micros()) {
    res = ser_read(&byte);
    if (res != r_DATA_READY) {
      if (res != r_ENONE) {
        printf("DEBUG: ser_read error %d\n", res);
      }
      continue;
    }

    if (byte == 0) {
      res = r_ENONE;
      if (recv_sync && packet_index > 0) {
        *len = packet_index;
        res = r_DATA_READY;

        if (len != NULL && data != NULL && packet_index != 0) {
          *data = malloc(*len);
          if (*data == NULL) res = r_EMEM;
          else memcpy(*data, packet_buff, *len);
        }
      }

      recv_sync = true;
      packet_index = 0;
      block_len = 0;
      block_read = 0;
      pending_zero = false;

      if (res == r_DATA_READY) return r_DATA_READY;
    }

    if (!recv_sync) continue;

    if (block_len == 0) {
      if (pending_zero) {
        if (packet_index < PACKET_BUFF_SIZE) {
          packet_buff[packet_index++] = 0;
        }
        pending_zero = false;
      }
      block_len = byte;
      block_read = 1;
    } else {
      if (packet_index < PACKET_BUFF_SIZE) {
        packet_buff[packet_index++] = byte;
      }
      block_read++;
    }

    if (block_read == block_len) {
      block_len = 0;
      if (block_read < 0xFF) {
        pending_zero = true;
      }
    }
  }

  recv_sync = false;
  packet_index = 0;
  block_len = 0;
  block_read = 0;
  pending_zero = false;

  *len = 0;
  *data = NULL;

  return r_ETIMEOUT;
}

result seh_transmit_buf(uint8_t len, const uint8_t* data) {
  result res;

  if (len > 254) return r_EARGS;

  uint8_t buf[254];

  res = ser_write(0x00);
  if (res != r_ENONE) return res;

  uint8_t prev_zero = len;
  
  for (int16_t i = len - 1; i >= 0; i--) {
    buf[i] = data[i];

    if (buf[i] == 0) {
      buf[i] = prev_zero - (uint8_t)i;
      prev_zero = (uint8_t)i;
    }
  }

  res = ser_write(prev_zero + 1);
  if (res != r_ENONE) return res;

  for (uint8_t i = 0; i < len; i++) {
    res = ser_write(buf[i]);
    if (res != r_ENONE) return res;
  }
  
  res = ser_write(0x00);
  if (res != r_ENONE) return res;

  return r_ENONE;
}

result seh_config(void) {
  result res;

  res = seh_transmit_buf(1, (uint8_t[]){SP_CMD_COMPAT_CODE});
  if (res != r_ENONE) return res;

  uint8_t len;
  uint8_t* data;

  res = seh_receive(&len, &data);

  if (res == r_DATA_READY) {
    if (len != 1 || *data != SP_COMPAT_CODE) {
      res = r_EDEVICE;
    } else {
      res = r_ENONE;
    }
    free(data);
  } else {
    res = r_EDEVICE;
  }

  for (int i = 0; i < 32; i++) {
    res = seh_transmit_buf(3, (uint8_t[]){SP_CMD_WRITE_IODIR, i, INPUT});
    if (res != r_ENONE) return res;
  }

  return res;
}

