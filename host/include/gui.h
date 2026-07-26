#ifndef GUI_H
#define GUI_H

#include <stdbool.h>
#include <raylib.h>

#define SCROLL_SPEED        -30

extern Font font;
extern int WINDOW_WIDTH;
extern int WINDOW_HEIGHT;
extern int FONT_SIZE;
extern int FONT_SPACING;

void gui_log(const char* log);
void gui_log_clear(void);
int gui_init(void);
void gui_close(void);
int gui_update(void);

#endif
