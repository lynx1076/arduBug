#include "rmu.h"
#include "gui.h"
#include "microui.h"
#include <raylib.h>
#include <stdlib.h>

static void draw_icon(mu_Command* cmd) {
  Color clr = { cmd->icon.color.r, cmd->icon.color.g, cmd->icon.color.b, cmd->icon.color.a };
  Rectangle r = { cmd->icon.rect.x, cmd->icon.rect.y, cmd->icon.rect.w, cmd->icon.rect.h };
  
  float cx = r.x + r.width / 2.0f;
  float cy = r.y + r.height / 2.0f;
  float size = 4.0f;

  switch (cmd->icon.id) {
    case MU_ICON_CLOSE: {
      DrawLineEx((Vector2){cx - size, cy - size}, (Vector2){cx + size, cy + size}, 1, clr);
      DrawLineEx((Vector2){cx + size, cy - size}, (Vector2){cx - size, cy + size}, 1, clr);
    } break;
    case MU_ICON_CHECK: {
      DrawLineEx((Vector2){cx - 3, cy}, (Vector2){cx - 1, cy + 3}, 1.5f, clr);
      DrawLineEx((Vector2){cx - 1, cy + 3}, (Vector2){cx + 4, cy - 3}, 1.5f, clr);
    } break;
    case MU_ICON_COLLAPSED: {
      DrawTriangle((Vector2){cx - 2, cy - 4}, (Vector2){cx - 2, cy + 4}, (Vector2){cx + 3, cy}, clr);
    } break;
    case MU_ICON_EXPANDED: {
      DrawTriangle((Vector2){cx - 4, cy - 2}, (Vector2){cx + 4, cy - 2}, (Vector2){cx, cy + 3}, clr);
    } break;
    default: {
      DrawRectangle(cx - 2, cy - 2, 4, 4, clr);
    } break;
  }
}

int rmu_text_width(mu_Font _font, const char *text, int len) {
  (void)_font;
  if (len == -1) len = TextLength(text);
  const char *sub = TextSubtext(text, 0, len);
  
  return MeasureTextEx(font, sub, FONT_SIZE, FONT_SPACING).x;
}

int rmu_text_height(mu_Font font) {
  (void)font;
  return FONT_SIZE; 
}

void rmu_input(mu_Context *ctx) {
  mu_input_mousemove(ctx, GetMouseX(), GetMouseY());
  
  float scroll = GetMouseWheelMove();
  if (scroll != 0) mu_input_scroll(ctx, 0, (int)(scroll * SCROLL_SPEED));

  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))   mu_input_mousedown(ctx, GetMouseX(), GetMouseY(), MU_MOUSE_LEFT);
  if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))  mu_input_mouseup(ctx, GetMouseX(), GetMouseY(), MU_MOUSE_LEFT);
  if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))  mu_input_mousedown(ctx, GetMouseX(), GetMouseY(), MU_MOUSE_RIGHT);
  if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) mu_input_mouseup(ctx, GetMouseX(), GetMouseY(), MU_MOUSE_RIGHT);
  
  int character = GetCharPressed();
  while (character > 0) {
    char str[2] = { (char)character, '\0' };
    mu_input_text(ctx, str);
    character = GetCharPressed();
  }
  if (IsKeyPressed(KEY_ENTER))
    mu_input_keydown(ctx, MU_KEY_RETURN);
  if (IsKeyReleased(KEY_ENTER))
    mu_input_keyup(ctx, MU_KEY_RETURN);

  if (IsKeyPressed(KEY_BACKSPACE))
    mu_input_keydown(ctx, MU_KEY_BACKSPACE);
  if (IsKeyReleased(KEY_BACKSPACE))
    mu_input_keyup(ctx, MU_KEY_BACKSPACE);

  if (IsKeyPressed(KEY_LEFT_SHIFT) || IsKeyPressed(KEY_RIGHT_SHIFT))
    mu_input_keydown(ctx, MU_KEY_SHIFT);
  if (IsKeyReleased(KEY_LEFT_SHIFT) || IsKeyReleased(KEY_RIGHT_SHIFT))
    mu_input_keyup(ctx, MU_KEY_SHIFT);

  if (IsKeyPressed(KEY_LEFT_CONTROL) || IsKeyPressed(KEY_RIGHT_CONTROL))
    mu_input_keydown(ctx, MU_KEY_CTRL);
  if (IsKeyReleased(KEY_LEFT_CONTROL) || IsKeyReleased(KEY_RIGHT_CONTROL))
    mu_input_keyup(ctx, MU_KEY_CTRL);

  if (IsKeyPressed(KEY_LEFT_ALT) || IsKeyPressed(KEY_RIGHT_ALT))
    mu_input_keydown(ctx, MU_KEY_ALT);
  if (IsKeyReleased(KEY_LEFT_ALT) || IsKeyReleased(KEY_RIGHT_ALT))
    mu_input_keyup(ctx, MU_KEY_ALT);
}

void rmu_render(mu_Context *ctx) {
  mu_Command* cmd = NULL;
  
  while (mu_next_command(ctx, &cmd)) {
    switch (cmd->type) {
      case MU_COMMAND_TEXT: {
        Color clr = { cmd->text.color.r, cmd->text.color.g, cmd->text.color.b, cmd->text.color.a };
        if (IsFontValid(font)) DrawTextEx(font, cmd->text.str, (Vector2){cmd->text.pos.x, cmd->text.pos.y}, FONT_SIZE, FONT_SPACING, clr);
        else DrawText(cmd->text.str, cmd->text.pos.x, cmd->text.pos.y, FONT_SIZE, clr);
      } break;
      case MU_COMMAND_RECT: {
        Color clr = { cmd->rect.color.r, cmd->rect.color.g, cmd->rect.color.b, cmd->rect.color.a };
        DrawRectangle(cmd->rect.rect.x, cmd->rect.rect.y, cmd->rect.rect.w, cmd->rect.rect.h, clr);
      } break;
      case MU_COMMAND_ICON: {
        draw_icon(cmd);
      } break;
      case MU_COMMAND_CLIP: {
        EndScissorMode();
        BeginScissorMode(cmd->clip.rect.x, cmd->clip.rect.y, cmd->clip.rect.w, cmd->clip.rect.h);
      } break;
    }
  }
  EndScissorMode();
}

