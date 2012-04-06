#ifndef UI_H
#define UI_H

#include "main.h"

#define UI_MIN_HEIGHT 5

typedef struct {
	WINDOW *win;
	int w, h;
} screen_t;

// +Main-----------------+
// | Top                 |
// |Separator------------|
// | Bottom              |
// +---------------------+

screen_t ui_win[4];

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
void ui_keypress(int c);
#endif
