#include "ucobs.h"
#include <stdint.h>

int ucobs_decode(uint8_t len, const uint8_t* src, uint8_t* dest) {
  uint8_t next_null = *src - 1;

  if (len == 0) return -1;

  src++;
  len--;

  for (uint8_t i = 0; i < len; i++) {
    uint8_t byte = src[i];
    if (byte == 0x00) return 1;

    if (i == next_null) {
      dest[i] = 0;
      next_null = i + byte;
      if (next_null > len || next_null == 0) return 1;
    } else {
      dest[i] = byte;
    }
  }

  return next_null == len ? len : -1;
}

int ucobs_encode(uint8_t len, const uint8_t* src, uint8_t* dest) {
  if (len > 254) return -1;

  uint8_t prev_zero = 0;

  *dest = len + 1;

  for (uint8_t i = 1; i <= len; i++) {
    dest[i] = src[i - 1];

    if (dest[i] == 0x00) {
      dest[prev_zero] = (uint8_t)i - prev_zero;
      prev_zero = i;
    }
  }

  dest[prev_zero] = len - prev_zero + 1;

  return len + 1;
}

