#ifndef MICROUI_BINDNIGS_H
#define MICROUI_BINDNIGS_H

#include <microui.h>


#define FONT_SIZE         18

void mub_render(mu_Context *ctx);
void mub_input(mu_Context *ctx);
void mub_draw(mu_Context *ctx);
int mub_text_width(mu_Font font, const char *text, int len);
int mub_text_height(mu_Font font);

#endif
