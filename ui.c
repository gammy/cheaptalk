#include "ui.h"

void ui_nullify(void) {
	int i;
	for(i = 0; i < 3; i++) {
		ui_win[i].win = NULL;
		ui_win[i].w = ui_win[i].h = 0;
	}
}

void ui_clear(void) {
	int i;
	for(i = 0; i < 3; i++)
		if(ui_win[i].win != NULL) 
			wclear(ui_win[i].win);
}

void ui_destroy(void) {
	int i;
	for(i = 0; i < 3; i++)
		if(ui_win[i].win != NULL)
			delwin(ui_win[i].win);
}

void ui_refresh(void) {
	int i;
	for(i = 0; i < 3; i++)
		if(ui_win[i].win != NULL)
			wrefresh(ui_win[i].win);
}

int ui_resized(void) {
	static int w, h;

	getmaxyx(stdscr, h, w);

	if(w != UI_MAIN.w || h != UI_MAIN.h) {
		UI_MAIN.w = w;
		UI_MAIN.h = h;
		return(1);
	} else
		return(0);
}

void ui_init(void) {
	// Destroy subwindows (main window is left intact)
	ui_destroy();
	ui_nullify();	

	int chatwin_height = ((int) (ceil(UI_MAIN.h / 2.0f))) - 1;
	clear();
	refresh(); // XXX *REQUIRED* for everything to work

	// Create subwindows
	// Top
	UI_CLIENT.win = newwin(chatwin_height, UI_MAIN.w, 0, 0);
	assert(UI_CLIENT.win != NULL);
	getmaxyx(UI_CLIENT.win, UI_CLIENT.h, UI_CLIENT.w);

	// Line
	UI_LINE.win = newwin(1, UI_MAIN.w, UI_CLIENT.h, 0);
	assert(UI_LINE.win != NULL);
	getmaxyx(UI_LINE.win, UI_LINE.h, UI_LINE.w);

	// Bottom
	UI_SERVER.win = newwin(chatwin_height, UI_MAIN.w, UI_CLIENT.h + UI_LINE.h, 0);
	assert(UI_SERVER.win != NULL);
	getmaxyx(UI_SERVER.win, UI_SERVER.h, UI_SERVER.w);

	ui_clear();

#ifdef DEBUG
	box(UI_CLIENT.win, 0, 0);
	box(UI_SERVER.win, 0, 0);
#endif
	whline(UI_LINE.win, ACS_HLINE, UI_LINE.w);
	ui_refresh();

}

#if 0

void ui_handle_wench(int s) {
	signal(s, SIG_IGN);
	ui_init();
	signal(s, ui_handle_wench);
}

#endif
