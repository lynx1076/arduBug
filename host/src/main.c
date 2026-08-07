#include "device.h"
#include "gui.h"
#include "result.h"
#include "utils.h"
#include <microui.h>
#include <raylib.h>
#include <stddef.h>


int main(int argc, char* argv[]) {
  (void)argc;
  (void)argv;

  debug_log(log_INFO, "Initializing");

  if (gui_init()) goto EXIT;

  while (true) {
    if (dev_update()) {
      if (_res == r_EMEM || _res == r_ESYS) {
        debug_log(log_CRIT, "Cannot recover from %s - Exiting", res_get_string(_res));
        goto EXIT;
      }
    }

    if (WindowShouldClose()) break;
    
    if (gui_update()) goto EXIT;

    if (program_should_close) {
      _res = r_ENONE;
      break;
    }
  }

EXIT:
  gui_close();

  return _res;
}

