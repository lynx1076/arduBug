#include "cli.h"
#include "result.h"
#include "utils.h"
#include <stdbool.h>
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

  while (true) {
    if (cli_update()) {
      if (_res == r_EMEM || _res == r_ESYS) {
        debug_log(log_CRIT, "Cannot recover from %s - Exiting", _res);
      }
    }
    sleep_ms(10);
  }
  
  quit(_res);
}

