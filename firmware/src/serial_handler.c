#include "serial.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>

static bool recv_sync = false;
static uint8_t packet_index = 0;
static uint8_t block_len = 0;
static uint8_t block_read = 0;
static bool pending_zero = false;

static uint8_t packet_buff[254];

void seh_receive(void (*onRead)(uint8_t, uint8_t*)) {
  uint8_t byte;
  if (ser_read(&byte) != 0) return;

  if (byte == 0) {
    if (recv_sync) {
      if (onRead) onRead(packet_index, packet_buff);
    }
    recv_sync = true;
    packet_index = 0;
    block_len = 0;
    block_read = 0;
    pending_zero = false;
    return;
  }

  if (!recv_sync) return;

  if (block_len == 0) {
    if (pending_zero) {
      if (packet_index < sizeof(packet_buff)) {
        packet_buff[packet_index++] = 0;
      }
      pending_zero = false;
    }
    block_len = byte;
    block_read = 1;
  } else {
    if (packet_index < sizeof(packet_buff)) {
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

void seh_transmit_buf(uint8_t len, const uint8_t* data) {
  if (len > 254) return;
  uint8_t buf[254];
  ser_write(0x00);

  uint8_t prev_zero = len;
  
  for (int16_t i = len - 1; i >= 0; i--) {
    buf[i] = data[i];

    if (buf[i] == 0) {
      buf[i] = prev_zero - (uint8_t)i;
      prev_zero = (uint8_t)i;
    }
  }

  ser_write(prev_zero + 1);

  for (uint8_t i = 0; i < len; i++) {
    ser_write(buf[i]);
  }
  
  ser_write(0x00);
}

