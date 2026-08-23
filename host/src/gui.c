#include "gui.h"
#include "commands.h"
#include "device.h"
#include "result.h"
#include "rmu.h"
#include "serial.h"
#include "serial_protocol.h"
#include "vector.h"
#include "utils.h"
#include <raylib.h>
#include <microui.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define CLI_LINE_LEN          512

#define RES_DIR               "res/"
#define FONT                  "OpenSans.ttf"

#define STATUSLINE_HEIGHT     25
#define STATUSLINE_LEN        128

#define WINDOW_MIN_WIDTH      600
#define WINDOW_MIN_HEIGHT     600

static mu_Context ctx;

static bool cmd_log_ready = false;
static Vec cmd_log;
static char input_buff[CLI_LINE_LEN];

static char statusline_buf[STATUSLINE_LEN + 1] = "";

static bool log_is_at_bottom = false;
static bool log_go_to_bottom = false;

int WINDOW_WIDTH = 800;
int WINDOW_HEIGHT = 600;

Font font;
int FONT_SIZE = 24;
int FONT_SPACING = 2;

static int handle_events(void) {
  program_should_close = WindowShouldClose();

  rmu_input(&ctx);

  RES_RETURN(r_ENONE, 0);
}

static int handle_gui_content(void) {
  mu_Rect cli_rect = mu_rect(0, 0, WINDOW_WIDTH * 0.65, WINDOW_HEIGHT * 0.9 - STATUSLINE_HEIGHT);
  mu_draw_rect(&ctx, cli_rect, mu_color(22, 22, 22, 255));

  mu_layout_row(&ctx, 2, (int[]){ cli_rect.w, -1 }, 0);
  mu_layout_begin_column(&ctx);

  mu_layout_set_next(&ctx, mu_rect(cli_rect.x, cli_rect.y + cli_rect.h + 4, cli_rect.w, FONT_SIZE * 2), 0);
  if (mu_textbox(&ctx, input_buff, CLI_LINE_LEN) == MU_RES_SUBMIT) {
    if (vec_push(&cmd_log, (void*)TextFormat("> %s", input_buff))) return -1;
    log_go_to_bottom = true;
    mu_set_focus(&ctx, ctx.last_id);

    if (cmd_execute(input_buff)) {
      gui_log(TextFormat("Executing command failed: %s", res_get_string(_res)));
    }

    input_buff[0] = '\0';
  }

  mu_layout_set_next(&ctx, mu_rect(0, 0, cli_rect.w, cli_rect.h), 0);
  mu_begin_panel(&ctx, "log");

  mu_Container *cmd_log_panel = mu_get_current_container(&ctx);
  if (log_go_to_bottom) {
    cmd_log_panel->scroll.y = cmd_log_panel->content_size.y;
    log_go_to_bottom = false;
  }
  log_is_at_bottom = cmd_log_panel->scroll.y == cmd_log_panel->content_size.y;
  
  for (size_t i = 0; i < cmd_log.count; i++) {
    mu_layout_row(&ctx, 1, (int[]){ -1 }, 0);
    mu_label(&ctx, vec_get(&cmd_log, i));
  }

  mu_end_panel(&ctx);
  mu_layout_end_column(&ctx);

  mu_layout_begin_column(&ctx);
  mu_layout_row(&ctx, 1, (int[]){-1}, 0);

  mu_label(&ctx, "--- Serial ---");
  if (ser_is_open()) {
    mu_label(&ctx, TextFormat("Connected to %s", ser_get_device()));
    mu_label(&ctx, TextFormat("Device is%s ready", dev_is_ready() ? "" : " not"));
  } else {
    mu_label(&ctx, "Not connected");
  }

  mu_label(&ctx, "--- CPU Info ---");
#ifndef DEBUG
  if (dev_is_ready()) {
    uint8_t state;
    uint8_t data;
    uint16_t addr;
    
    if (!dev_get_cpu_en()) {
      mu_label(&ctx, "CPU is not enabled");
    } else if (dev_get_ext_clk_en()) {
      if (dev_get_cpu_state(&state) | dev_read_databus(&data) | dev_read_addrbus(&addr)) {
        mu_label(&ctx, TextFormat("Unable to get bus state: %s", res_get_string(_res)));
      } else {
        bool tbo = state & (1 << SP_STATE_DEV_TBO);
        bool irq = state & (1 << SP_STATE_IRQ);
        bool nmi = state & (1 << SP_STATE_NMI);
        bool ml = state & (1 << SP_STATE_ML);
        bool vp = state & (1 << SP_STATE_VP);
        bool cpu_sync = state & (1 << SP_STATE_SYNC);
        bool writing = state & (1 << SP_STATE_WRITING);

        mu_label(&ctx, TextFormat("CPU is %s 0x%02x at 0x%04x", writing ? "writing" : "reading", data, addr));

        mu_label(&ctx, TextFormat("Bus is%s owned", tbo ? "" : " not"));
        if (cpu_sync) mu_label(&ctx, TextFormat("CPU is fetching an opcode"));
        if (vp) mu_label(&ctx, TextFormat("CPU is fetching a vector"));
        if (ml) mu_label(&ctx, TextFormat("CPU has locked the memory"));
        if (irq) mu_label(&ctx, "IRQ is active");
        if (nmi) mu_label(&ctx, "NMI is active");
      }
    } else {
      mu_label(&ctx, "Internal clock active");
    }
  } else {
    mu_label(&ctx, "Device is not ready");
  }
#else
  mu_label(&ctx, "Deactivated for debug");
#endif

  mu_layout_end_column(&ctx);

  mu_layout_set_next(&ctx, mu_rect(10, WINDOW_HEIGHT - STATUSLINE_HEIGHT * 1.2, WINDOW_WIDTH, STATUSLINE_HEIGHT), 0);
  mu_label(&ctx, statusline_buf);

  RES_RETURN(r_ENONE, 0);
}

void gui_log(const char* log) {
  if (!cmd_log_ready) printf("Attempted gui log but logger not ready.\ngui log: %s", log);
  else if (vec_push(&cmd_log, (void*)TextSubtext(log, 0, CLI_LINE_LEN))) {
    printf("Attempted gui log but logger failed.\ngui log: %s", log);
  }
  if (log_is_at_bottom) log_go_to_bottom = true;
}

void gui_set_statusline(const char* text) {
  strncpy(statusline_buf, text, STATUSLINE_LEN);
  statusline_buf[STATUSLINE_LEN] = '\0';
}

void gui_log_clear(void) {
  if (!cmd_log_ready) return;
  vec_clear(&cmd_log);
}

int gui_init(void) {
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "ardubug host");
  SetExitKey(KEY_NULL);

  if (!IsWindowReady()) {
    RES_RETURN(r_EUNKNOWN, -1);
  }

  WINDOW_WIDTH = GetScreenWidth();
  WINDOW_HEIGHT = GetScreenHeight();

  SetTargetFPS(30);
  
  mu_init(&ctx);
  ctx.text_width = &rmu_text_width;
  ctx.text_height = &rmu_text_height;

  font = LoadFontEx(RES_DIR FONT, FONT_SIZE, NULL, 0);
  if (IsFontValid(font)) {
    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
  } else {
    debug_log(log_ERR, "Could not load font from %s", RES_DIR FONT);
  }

  if (vec_init(&cmd_log, CLI_LINE_LEN)) return -1;
  else cmd_log_ready = true;

  RES_RETURN(r_ENONE, 0);
}

void gui_close(void) {
  UnloadFont(font);
  CloseWindow();
}

int gui_update(void) {
  if (handle_events()) return -1;

  WINDOW_WIDTH = GetScreenWidth();
  WINDOW_HEIGHT = GetScreenHeight();

  mu_begin(&ctx);

  if (mu_begin_window_ex(&ctx, "main_window", mu_rect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT), MU_OPT_NOCLOSE | MU_OPT_NOTITLE | MU_OPT_NORESIZE)) {
    mu_Container* cnt = mu_get_current_container(&ctx);
    cnt->rect = mu_rect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    if (WINDOW_WIDTH < WINDOW_MIN_WIDTH || WINDOW_HEIGHT < WINDOW_MIN_HEIGHT) {
      mu_layout_set_next(&ctx, mu_rect(2, 2, WINDOW_WIDTH, WINDOW_HEIGHT), 0);
      mu_label(&ctx, TextFormat("Window too small - increase to atleast %ix%i", WINDOW_MIN_WIDTH, WINDOW_MIN_HEIGHT));
    } else {
      if (handle_gui_content()) return -1;
    }

    mu_end_window(&ctx);
  }

  mu_end(&ctx);

  BeginDrawing();
  ClearBackground(GRAY);

  rmu_render(&ctx);

  EndDrawing();

  RES_RETURN(r_ENONE, 0);
}

