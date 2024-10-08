#ifndef UI_H
#define UI_H

#include "main.h"

#define UI_MIN_HEIGHT      5

#define UI_KEY_ESC         27
#define UI_KEY_BACKSPACE   127
#define UI_KEY_EOT         4
#define UI_KEY_CR          13
#define UI_KEY_LF          10

typedef struct {
	WINDOW *win;
	int w, h;
} screen_t;

// +Main-----------------+
// | Top                 |
// |Separator------------|
// | Bottom              |
// +---------------------+

extern screen_t ui_win[4]; // ui.c

#define UI_TOP		(ui_win[0])
#define UI_SEP		(ui_win[1])
#define UI_BOT		(ui_win[2])
#define UI_MAIN		(ui_win[3])

void ui_nullify(void);
void ui_clear(void);
void ui_destroy(void);
void ui_refresh(void);
int  ui_resized(void);
void ui_init(void);
void ui_resize(void);
#if 0
void ui_handle_wench(int s);
#endif
void ui_keypress(screen_t *screen, int c);
#endif
