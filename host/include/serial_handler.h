#ifndef SERIAL_HANDLER_H
#define SERIAL_HANDLER_H

#include "result.h"
#include <stdint.h>


/*
 * Initalize internal variables and buffers
*/
result seh_init(void);

/*
 * Free internal variables and buffers
*/
void seh_free(void);

/*
 * Update internal variables
*/
void seh_update(void);

/*
 * Receive data from serial and decode from COBS to normal array
 * Returns a pointer to the received and decoded data, needs to be freed by caller
*/
result seh_receive(uint8_t* len, uint8_t** data);

/*
 * Transmit a buffer of data
 * Function encodes data to COBS
*/
result seh_transmit_buf(uint8_t len, const uint8_t* data);

/*
 * Configure the device and initalize it to a known state
*/
result seh_config(void);

#endif
