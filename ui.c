#include "ui.h"

void ui_nullify(void) {
	unsigned int i;
	for(i = 0; i < 3; i++) {
		ui_win[i].win = NULL;
		ui_win[i].w = ui_win[i].h = 0;
	}
}

void ui_clear(void) {
	unsigned int i;
	for(i = 0; i < 3; i++)
		if(ui_win[i].win != NULL) 
			wclear(ui_win[i].win);
}

void ui_destroy(void) {
	unsigned int i;
	for(i = 0; i < 3; i++)
		if(ui_win[i].win != NULL)
			delwin(ui_win[i].win);
}

void ui_refresh(void) {
	unsigned int i;
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

	if(chatwin_height < UI_MIN_HEIGHT)
		chatwin_height = UI_MIN_HEIGHT;

	clear();
	refresh(); // XXX Required for everything to work

	// Create subwindows
	// Top
	UI_TOP.win = newwin(chatwin_height, UI_MAIN.w, 0, 0);
	assert(UI_TOP.win != NULL);
	getmaxyx(UI_TOP.win, UI_TOP.h, UI_TOP.w);
	//idlok(UI_TOP.win, TRUE);
	scrollok(UI_TOP.win, TRUE);

	// Line
	UI_LINE.win = newwin(1, UI_MAIN.w, UI_TOP.h, 0);
	assert(UI_LINE.win != NULL);
	getmaxyx(UI_LINE.win, UI_LINE.h, UI_LINE.w);

	// Bottom
	UI_BOT.win = newwin(chatwin_height, UI_MAIN.w, UI_TOP.h + UI_LINE.h, 0);
	assert(UI_BOT.win != NULL);
	getmaxyx(UI_BOT.win, UI_BOT.h, UI_BOT.w);
	//idlok(UI_BOT.win, TRUE);
	scrollok(UI_BOT.win, TRUE);

	ui_clear();

#if 0
	box(UI_TOP.win, 0, 0);
	box(UI_BOT.win, 0, 0);
#endif
	whline(UI_LINE.win, ACS_HLINE, UI_LINE.w);
	ui_refresh();

}

void ui_resize(void) {
	//getmaxyx(UI_MAIN.win, UI_MAIN.h, UI_MAIN.w);

	int chatwin_height = ((int) (ceil(UI_MAIN.h / 2.0f))) - 1;

	if(chatwin_height < UI_MIN_HEIGHT)
		chatwin_height = UI_MIN_HEIGHT;

	clear();
	refresh(); // XXX Required for everything to work


	if(wresize(UI_TOP.win, chatwin_height, UI_MAIN.w) != OK)
		abort();
	//getmaxyx(UI_TOP.win, UI_TOP.h, UI_TOP.w);
	UI_TOP.w = UI_MAIN.w;
	UI_TOP.h = chatwin_height;
	mvwin(UI_TOP.win, 0, 0);

	if(wresize(UI_LINE.win, 1, UI_MAIN.w) != OK)
		abort();
	//getmaxyx(UI_LINE.win, UI_LINE.h, UI_LINE.w);
	UI_LINE.w = UI_MAIN.w;
	UI_LINE.h = 1;
	mvwin(UI_LINE.win, UI_TOP.h, 0);
	wclear(UI_LINE.win);
	whline(UI_LINE.win, ACS_HLINE, UI_LINE.w);

	if(wresize(UI_BOT.win, chatwin_height, UI_MAIN.w) != OK)
		abort();
	//getmaxyx(UI_BOT.win, UI_BOT.h, UI_BOT.w);
	UI_BOT.w = UI_MAIN.w;
	UI_BOT.h = chatwin_height;
	mvwin(UI_BOT.win, UI_TOP.h + UI_LINE.h, 0);

#if 0
	box(UI_TOP.win, 0, 0);
	box(UI_BOT.win, 0, 0);
#endif
	ui_refresh();
}

#if 0

void ui_handle_wench(int s) {
	signal(s, SIG_IGN);
	ui_init();
	signal(s, ui_handle_wench);
}

#endif
