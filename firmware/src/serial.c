#include "serial.h"
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>


static uint8_t ring_buf[SER_RX_BUF_SIZE];

static volatile uint8_t head = 0;
static volatile uint8_t tail = 0;

void ser_init() {
  uint16_t ubrr0_val = (F_CPU + 4UL * SER_BAUD_RATE) / (8UL * SER_BAUD_RATE) - 1;

  UBRR0H = ubrr0_val >> 8;
  UBRR0L = ubrr0_val & 0xFF;

  UCSR0A |= _BV(U2X0);
  UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
  UCSR0B = _BV(RXEN0) | _BV(TXEN0) | _BV(RXCIE0);
}

uint8_t ser_available(void) {
  if (head == tail) return 0;
  else if (head > tail) return head - tail;
  else return SER_RX_BUF_SIZE - tail + head;
}

int ser_read(uint8_t* byte) {
  if (!ser_available()) return -1;
  if (byte) *byte = ring_buf[tail];
  if (++tail >= SER_RX_BUF_SIZE) tail = 0;
  return 0;
}

void ser_write(uint8_t byte) {
  while (!(UCSR0A & _BV(UDRE0)));

  UDR0 = byte;
}

void ser_write_buf(uint8_t len, const uint8_t* buf) {
  for (uint8_t i = 0; i < len; i++) {
    ser_write(buf[i]);
  }
}

void ser_write_str(const char* format, ...) {
  char buffer[64];
  va_list args;

  va_start(args, format);
  vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);

  for (uint8_t i = 0; buffer[i] != '\0'; i++) {
    ser_write(buffer[i]);
  }
}

ISR(USART_RX_vect) {
  ring_buf[head] = UDR0;

  if (++head >= SER_RX_BUF_SIZE) head = 0;
}

