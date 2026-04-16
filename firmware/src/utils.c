#include "utils.h"
#include <util/delay.h>


void(*_reset)(void) = 0;

void reset(void) {
  _delay_ms(200);
  _reset();
}

