#include <raylib.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include "microui_bindings.h"
#include "microui.h"
#include "result.h"
#include "serial.h"
#include "serial_handler.h"
#include "ui.h"


mu_Context ctx;

int main(int argc, char* argv[]) {
  (void)argc;
  (void)argv;

  SetTraceLogLevel(LOG_NONE);

  InitWindow(WIDTH, HEIGHT, WINDOW_TITLE);
  SetTargetFPS(30);
  SetExitKey(KEY_NULL);

  if (ser_init() != r_ENONE) {
    goto EXIT_ERR;
  }

  if (seh_init() != r_ENONE) {
    goto EXIT_ERR;
  }

  mu_init(&ctx);
  ctx.text_width = mub_text_width;
  ctx.text_height = mub_text_height;

  while (!WindowShouldClose()) {
    ser_update();
    seh_update();
    mub_input(&ctx);

    mu_begin(&ctx);
    if (mu_begin_window_ex(&ctx, WINDOW_TITLE, mu_rect(0, 0, WIDTH, HEIGHT), MU_FLAGS)) {
      if (ui_process_and_draw(&ctx) != r_ENONE) break;
      mu_end_window(&ctx);
    }
    mu_end(&ctx);

    BeginDrawing();
    
    ClearBackground(*(Color*)(&COLOR_BG));
    
    mub_render(&ctx);

    EndDrawing();
  }

  CloseWindow();
  seh_free();
  ser_free();

  return 0;

EXIT_ERR:
  CloseWindow();
  seh_free();
  ser_free();

  return 1;
}

