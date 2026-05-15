#include "ucobs.h"
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>


int ucobs_decode(uint8_t length, const uint8_t* data, uint8_t* dest) {
  if (*data == 0) return -1;
  if (length == 0) return -1;

  size_t next_null = *data - 1;
  data++;
  size_t len = length - 1;

  for (size_t i = 0; i < len; i++) {
    uint8_t byte = data[i];

    if (i == next_null) {
      dest[i] = 0;
      next_null = i + byte;
      if (next_null > len || byte == 0) return -1;
    } else {
      dest[i] = byte;
    }
  }

  return next_null == len ? (int)len : -1;
}

int ucobs_encode(uint8_t length, const uint8_t* data, uint8_t* dest) {
  if (length == 0 || length > 254) return -1;

  size_t prev_zero = 0;
  uint8_t tmp = data[0];
  uint8_t tmp_old;

  for (size_t i = 0; i < length; i++) {
    tmp_old = tmp;
    tmp = data[i + 1];
    dest[i + 1] = tmp_old;

    if (tmp_old == 0x00) {
      dest[prev_zero] = (uint8_t)(i - prev_zero + 1);
      prev_zero = i + 1;
    }
  }

  dest[prev_zero] = (uint8_t)(length - prev_zero + 1);

  return (int)(length + 1);
}

