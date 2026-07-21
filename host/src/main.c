#include "cli.h"
#include "result.h"
#include "utils.h"
#include "serial.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include <ncurses.h>

int main(int argc, char* argv[]) {
  (void)argc;
  (void)argv;

  debug_log(log_INFO, "Initializing ncurses");

  initscr();
  cbreak();
  echo();
  scrollok(stdscr, true);

  size_t timer = millis();

  while (true) {
    if (cli_update()) {
      if (_res == r_EMEM || _res == r_ESYS) {
        debug_log(log_CRIT, "Cannot recover from %s - Exiting", res_get_string(_res));
      }
    }

    if (timer + 1000 < millis()) {
      timer = millis();
      print("A");
    }
    
    if (ser_update()) {
      if (_res == r_EMEM || _res == r_ESYS) {
        debug_log(log_CRIT, "Cannot recover from %s - Exiting", res_get_string(_res));
      }
    }

    sleep_ms(10);
  }
  
  quit(_res);
}

