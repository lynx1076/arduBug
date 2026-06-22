#include "mcu_io.h"

typedef struct {
  
} mio_Pin;


uint8_t mio_write(uint8_t pin, PinState state) {
#define PIN_CASE(IDX, PORT)
  switch (pin) {
  }
}

uint8_t mio_setpullup(uint8_t pin, PinState state);
uint8_t mio_setmode(uint8_t pin, PinMode mode);
uint8_t mio_read(uint8_t pin, PinState* state);

