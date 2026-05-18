#include "cli.h"
#include "result.h"
#include "utils.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
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
      if (_res == r_EMEM || _res == r_ESYS || _res == r_EDOUBLE_INIT || _res == r_ENOT_INIT) break;
    }
    sleep_ms(10);
  }
  
  quit(_res);
}

