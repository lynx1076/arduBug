#ifndef UI_H
#define UI_H

#include "microui.h"
#include "result.h"

#define WIDTH             1200
#define HEIGHT            800
#define WINDOW_TITLE      "arduBug"

#define UI_LOG_LINES      14
#define UI_LOG_LEN        60

#define MU_FLAGS          (MU_OPT_NORESIZE | MU_OPT_NOTITLE | MU_OPT_NOFRAME)

#define COLOR_BG          (mu_Color){0x20, 0x20, 0x60, 0xFF}

result ui_process_and_draw(mu_Context *ctx);
void ui_log(const char* msg);

#endif
