#include "serial.h"
#include "serial_protocol.h"
#include <avr/interrupt.h>
#include <stdint.h>
#include <string.h>


static uint8_t ring_buf[SER_RX_BUFF_SIZE];

static volatile uint8_t head = 0;
static volatile uint8_t tail = 0;

void ser_init(uint32_t baudrate) {
  uint16_t ubrr0_val = (F_CPU / 16 / baudrate) - 1;

  UBRR0H = ubrr0_val >> 8;
  UBRR0L = ubrr0_val & 0xFF;

  UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
  UCSR0B = _BV(RXEN0) | _BV(TXEN0) | _BV(RXCIE0);
}

uint8_t ser_available(void) {
  if (head == tail) return 0;
  else if (head > tail) return head - tail;
  else return SER_RX_BUFF_SIZE - tail + head;
}

int16_t ser_readc(void) {
  if (!ser_available()) return -1;
  uint8_t c = ring_buf[tail++];
  if (tail >= SER_RX_BUFF_SIZE) tail = 0;
  return c;
}

uint8_t ser_read(uint8_t* buf, uint8_t buf_size) {
  uint8_t read = 0;

  while (ser_available() && buf_size--) {
    if (buf != NULL) *buf++ = ring_buf[tail];
    if (++tail >= SER_RX_BUFF_SIZE) tail = 0;
    read++;
  }

  return read;
}

void ser_write_raw(uint8_t byte) {
  while (!(UCSR0A & _BV(UDRE0)));
  UDR0 = byte;
}

void ser_write(uint8_t c) {
  switch (c) {
    case SP_SIG_SYNC:
    case SP_SIG_ESC:
    case SP_SIG_NACK:
    case SP_SIG_ACK: {
      ser_write_raw(SP_SIG_ESC);
    } break;
    default:
      break;
  }

  ser_write_raw(c);
}

void ser_write_string(const char* str, uint8_t len) {
  while (len--) {
    ser_write((uint8_t)*str++);
  }
}

ISR(USART_RX_vect) {
  ring_buf[head] = UDR0;

  if (++head >= SER_RX_BUFF_SIZE) head = 0;

  if (head == tail) {
    if (++tail >= SER_RX_BUFF_SIZE) tail = 0;
  }
}

