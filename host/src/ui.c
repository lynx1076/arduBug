#include "ui.h"
#include "result.h"
#include "utils.h"
#include "microui.h"
#include "microui_bindings.h"
#include "serial.h"
#include "vector.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

char* info_line = NULL;
char logs[UI_LOG_LINES][UI_LOG_LEN + 1] = {};

result ui_process_and_draw(mu_Context *ctx) {
  result res;

  mu_layout_row(ctx, 2, (int[]){750, -1}, 0);
  mu_layout_begin_column(ctx);
  mu_draw_rect(ctx, mu_rect(10, 10, 400, HEIGHT - 20), mu_color(40, 40, 40, 255));
  mu_layout_end_column(ctx);

  mu_layout_begin_column(ctx);
  mu_layout_row(ctx, 1, (int[]){-1}, 0);
  mu_label(ctx, "Serial ports:");

  Vec ports;
  if (ser_scan_ports(&ports) != r_ENONE) {
    debug_log(log_ERR, "Failed scanning for ports");
  }

  for (size_t i = 0; i < ports.count; i++) {
    char* new_port_name = vec_get(&ports, i);
    if (mu_button(ctx, new_port_name)) {
      char* open_port_name = ser_get_current_port();

      if (open_port_name == NULL || strcmp(new_port_name, open_port_name) != 0) {
        ser_close();
        res = ser_open(new_port_name);
        if (res == r_ENONE) {
          debug_log(log_OK, "Opened and configured port %s", new_port_name);
        } else {
          debug_log(log_ERR, "Failed to open port %s -> %s", new_port_name, res_get_string(res));
        }
      }
    }
  }

  if (mu_button(ctx, "Close")) {
    if (ser_is_open()) {
      debug_log(log_OK, "Closing port %s", ser_get_current_port());
      ser_close();
    } else {
      debug_log(log_WARN, "No port to close");
    }
  }

  for (int i = UI_LOG_LINES - 1; i >= 0; i--) {
    if (logs[i][0] == '\0') continue;
    int y_offset = (FONT_SIZE + 4) * i;
    mu_layout_set_next(ctx, mu_rect(750, 500 - y_offset, WIDTH - 750, FONT_SIZE), 0);
    mu_label(ctx, logs[i]);
  }

  if (ser_just_closed()) {
    debug_log(log_WARN, "Port closed");
  }

  if (ser_just_opened()) {
    debug_log(log_INFO, "Port opened");
  }

  mu_layout_end_column(ctx);

  return r_ENONE;
}

void ui_log(const char* msg) {
  for (int i = UI_LOG_LINES - 1; i > 0; i--) {
    memcpy(logs[i], logs[i - 1], UI_LOG_LEN);
  }

  snprintf((char*)&logs[0], UI_LOG_LEN, "%s", msg);
}

