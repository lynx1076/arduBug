#ifndef UCOBS_H
#define UCOBS_H

#include <stdint.h>

/*
 * uCOBS (micro-COBS) is based around the basic idea of the original algorithm while also
 * not implementing certain features to increase stability and code-predictability at the cost of certain performance boosts like
 * 0xFF bytes. Framing zeroes should always be sent but are not necessary.
*/

int ucobs_decode(uint8_t len, const uint8_t* src, uint8_t* dest);
int ucobs_encode(uint8_t len, const uint8_t* src, uint8_t* dest);

#endif
