#include "microui_bindings.h"
#include <raylib.h>
#include <microui.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

void mub_render(mu_Context *ctx) {
  mu_Command *cmd = NULL;

  while (mu_next_command(ctx, &cmd)) {
    switch (cmd->type) {
    case MU_COMMAND_RECT:
      DrawRectangle(
        cmd->rect.rect.x,
        cmd->rect.rect.y,
        cmd->rect.rect.w,
        cmd->rect.rect.h,
        (Color){cmd->rect.color.r, cmd->rect.color.g,
            cmd->rect.color.b, cmd->rect.color.a});
      break;
    case MU_COMMAND_TEXT:
      DrawText(
        cmd->text.str,
        cmd->text.pos.x,
        cmd->text.pos.y,
        FONT_SIZE,
        WHITE);
      break;
    case MU_COMMAND_CLIP:
      BeginScissorMode(
        cmd->clip.rect.x,
        cmd->clip.rect.y,
        cmd->clip.rect.w,
        cmd->clip.rect.h);
      break;
    }
  }

  EndScissorMode();
}

void mub_input(mu_Context *ctx) {
  Vector2 mouse = GetMousePosition();

  mu_input_mousemove(ctx, mouse.x, mouse.y);

  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    mu_input_mousedown(ctx, mouse.x, mouse.y, MU_MOUSE_LEFT);

  if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
    mu_input_mouseup(ctx, mouse.x, mouse.y, MU_MOUSE_LEFT);

  float wheel = GetMouseWheelMove();
  if (wheel != 0)
    mu_input_scroll(ctx, 0, wheel * -30);
}

int mub_text_width(mu_Font font, const char *text, int len) {
  (void)font;
  if (len == -1) { len = strlen(text); }
  return MeasureText(text, FONT_SIZE);
}

int mub_text_height(mu_Font font) {
  (void)font;
  return FONT_SIZE;
}

