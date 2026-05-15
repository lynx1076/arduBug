/*
 * uCOBS (micro-COBS) is based around the basic idea of the original COBS algorithm while also
 * not implementing certain features like 0xFF bytes to increase stability and code-predictability at the cost of performance
*/

#ifndef UCOBS_H
#define UCOBS_H

#include <stdint.h>
#include <stdlib.h>


// Maximum length of UCOBS data in packet
#define UCOBS_MAX_DATA_LEN                    254

// Maximum length of UCOBS packet with no framing-bytes
#define UCOBS_MAX_PACKET_LEN_NO_FRAME         UCOBS_MAX_DATA_LEN + 1

// UCOBS framing-bytes length
#define UCOBS_LEN_FRAME                       2

// Maximum length of ucobs packet with framing-bytes
#define UCOBS_MAX_PACKET_LEN                  UCOBS_MAX_PACKET_LEN_NO_FRAME + UCOBS_LEN_FRAME

/*
 * The memory used by the buffer and dest may be the same
 * Zero-framing must be ensured by caller
 * Buffer may not be longer than 254 bytes
 *
 * Parameters must not be null
 *
 * Returns number of decoded bytes on success
 * Returns -1 on failure
*/
int ucobs_decode(uint8_t length, const uint8_t* data, uint8_t* dest);

/*
 * The memory used by the buffer and dest may be the same
 * Zero-framing must be ensured by caller
 * Buffer may not be longer than 254 bytes
 *
 * Parameters must not be null
 *
 * Returns message length on success
 * Returns -1 on failure
*/
int ucobs_encode(uint8_t length, const uint8_t* data, uint8_t* dest);

#endif
