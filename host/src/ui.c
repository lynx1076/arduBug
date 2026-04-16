#include "ui.h"
#include "result.h"
#include "serial_handler.h"
#include "microui.h"
#include "microui_bindings.h"
#include "raylib.h"
#include "serial.h"
#include "serial_protocol.h"
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

  int ports;
  if (ser_scan_ports(&ports)) {
    ui_log("Failed scanning for ports", ull_ERR);
  }

  for (int i = 0; i < ports; i++) {
    char* new_port_name = ser_get_port_name(i);
    if (mu_button(ctx, new_port_name)) {
      char* open_port_name = ser_get_open();

      if (open_port_name == NULL || strcmp(new_port_name, open_port_name) != 0) {
        ser_close();
        if (ser_open_by_id(i) == r_ENONE) {
          ui_log((char*)TextFormat("Opened and configured port %s", new_port_name), ull_OK);
        } else {
          ui_log((char*)TextFormat("Failed to open port %s", new_port_name), ull_ERR);
        }
      }
    }
  }

  if (mu_button(ctx, "Close")) {
    if (ser_is_open()) {
      ui_log((char*)TextFormat("Closing port %s", ser_get_open()), ull_OK);
      ser_close();
    } else {
      ui_log("No port to close", ull_WARN);
    }
  }

  for (int i = UI_LOG_LINES - 1; i >= 0; i--) {
    if (logs[i][0] == '\0') continue;
    int y_offset = (FONT_SIZE + 4) * i;
    mu_layout_set_next(ctx, mu_rect(750, 500 - y_offset, WIDTH - 750, FONT_SIZE), 0);
    mu_label(ctx, logs[i]);
  }

  if (ser_just_closed()) {
    ui_log("Port closed", ull_WARN);
  }

  if (ser_just_opened()) {
    ui_log("Port opened", ull_INFO);
    seh_transmit_buf(1, (uint8_t[]){SP_CMD_VERSION_TEXT});
    uint8_t len;
    uint8_t* data;
    
    res = seh_receive(&len, &data);
    if (res == r_DATA_READY) {
      ui_log(TextFormat("Version: %.*s\n", len, data), ull_INFO);
      free(data);
    } else {
      printf("Could not retrieve version info\n");
    }
  }

  mu_layout_end_column(ctx);

  return r_ENONE;
}

void ui_log(const char* msg, ui_log_level ull) {
  for (int i = UI_LOG_LINES - 1; i > 0; i--) {
    memcpy(logs[i], logs[i - 1], UI_LOG_LEN);
  }
  char* ull_prefix;

  switch (ull) {
  case ull_INFO:
    ull_prefix = "INFO";
    break;
  case ull_ERR:
    ull_prefix = "ERR";
    break;
  case ull_CRIT:
    ull_prefix = "CRIT";
    break;
  case ull_WARN:
    ull_prefix = "WARN";
    break;
  case ull_OK:
    ull_prefix = "OK";
    break;
  default:
    ull_prefix = "...";
    break;
  }

  snprintf((char*)&logs[0], UI_LOG_LEN, "[%s] %s", ull_prefix, msg);
  printf("[%s] %s\n", ull_prefix, msg);
}

