#include "serial.h"
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdint.h>


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

uint8_t ser_read(uint8_t* byte) {
  if (!ser_available()) return -1;
  if (byte) *byte = ring_buf[tail];
  if (++tail >= SER_RX_BUF_SIZE) tail = 0;
  return 0;
}

void ser_write(uint8_t byte) {
  while (!(UCSR0A & _BV(UDRE0)));
  UDR0 = byte;
}

ISR(USART_RX_vect) {
  ring_buf[head] = UDR0;

  if (++head >= SER_RX_BUF_SIZE) head = 0;
}

