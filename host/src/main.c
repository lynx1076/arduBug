#include "microui_bindings.h"
#include "serial.h"
#include "utils.h"
#include "microui.h"
#include "result.h"
#include "ui.h"
#include <raylib.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

mu_Context ctx;

int main_loop(void) {
  mub_input(&ctx);

  mu_begin(&ctx);
  if (mu_begin_window_ex(&ctx, WINDOW_TITLE, mu_rect(0, 0, WIDTH, HEIGHT), MU_FLAGS)) {
    if (ser_update() != r_ENONE) return -1;
    if (ui_process_and_draw(&ctx) != r_ENONE) return -1;
    mu_end_window(&ctx);
  }
  mu_end(&ctx);

  BeginDrawing();
  
  ClearBackground(*(Color*)(&COLOR_BG));
  
  mub_render(&ctx);

  EndDrawing();

  return 0;
}


int main(int argc, char* argv[]) {
  (void)argc;
  (void)argv;

  debug_log(log_INFO, "Initializing");

  SetTraceLogLevel(LOG_NONE);

  InitWindow(WIDTH, HEIGHT, WINDOW_TITLE);
  SetTargetFPS(30);
  SetExitKey(KEY_NULL);

  debug_log(log_OK, "Raylib initialized");

  mu_init(&ctx);
  ctx.text_width = mub_text_width;
  ctx.text_height = mub_text_height;

  debug_log(log_OK, "MU initialized");

  debug_log(log_OK, "Initialization complete");

  while (!WindowShouldClose()) {
    if (main_loop()) goto EXIT_ERR;
  }

  CloseWindow();

  return 0;

EXIT_ERR:
  CloseWindow();

  return 1;
}

