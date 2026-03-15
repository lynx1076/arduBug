#include <stdbool.h>
#include <stdio.h>
#include "serial.h"

int main(int argc, char* argv[]) {
  (void)argc;
  (void)argv;

  ser_init();

  int count = ser_scan_ports();
  if (count < 0) {
    ser_free();
    return -1;
  }

  for (int i = 0; i < count; i++) {
    printf("%s\n", ser_get_port_name(i));
  }

  ser_free();

  return 0;
}

