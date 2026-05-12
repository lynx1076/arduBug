#include "cli_handler.h"
#include "utils.h"
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
  (void)argc;
  (void)argv;

  debug_log(log_INFO, "Starting...");

  while (true) {
    clh_update();
    sleep_ms(10);
  }

  return 0;

EXIT_ERR:

  return 1;
}

