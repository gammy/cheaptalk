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

	ui_destroy();
	ui_nullify();	

	int h = (int) UI_MAIN.h / 2.0f;

	if(h < UI_MIN_HEIGHT)
		h = UI_MIN_HEIGHT;

	clear();
	refresh(); // XXX Required for everything to work

	UI_TOP.win = newwin(h, UI_MAIN.w, 0, 0);
	assert(UI_TOP.win != NULL);
	getmaxyx(UI_TOP.win, UI_TOP.h, UI_TOP.w);
	//idlok(UI_TOP.win, TRUE);
	scrollok(UI_TOP.win, TRUE);

	UI_SEP.win = newwin(1, UI_MAIN.w, UI_TOP.h, 0);
	assert(UI_SEP.win != NULL);
	getmaxyx(UI_SEP.win, UI_SEP.h, UI_SEP.w);

	UI_BOT.win = newwin(h - ((UI_MAIN.h & 1) == 0 ? 1 : 0), UI_MAIN.w, UI_TOP.h + UI_SEP.h, 0);
	assert(UI_BOT.win != NULL);
	getmaxyx(UI_BOT.win, UI_BOT.h, UI_BOT.w);
	//idlok(UI_BOT.win, TRUE);
	scrollok(UI_BOT.win, TRUE);

	ui_clear();

#if 0
	box(UI_TOP.win, 0, 0);
	box(UI_BOT.win, 0, 0);
#endif
	whline(UI_SEP.win, ACS_HLINE, UI_SEP.w);

	wmove(UI_TOP.win, UI_TOP.h - 1, 0);
	wmove(UI_BOT.win, UI_BOT.h - 1, 0);

	ui_refresh();

}

void ui_resize(void) {

	int h = (int) UI_MAIN.h / 2.0f;

	if(h < UI_MIN_HEIGHT)
		h = UI_MIN_HEIGHT;

	UI_TOP.w = UI_SEP.w = UI_BOT.w = UI_MAIN.w;

	UI_TOP.h = h;
	UI_SEP.h = 1;
	UI_BOT.h = h - ((UI_MAIN.h & 1) == 0 ? 1 : 0);

	clear();
	refresh(); // XXX Required for everything to work

	wresize(UI_TOP.win, UI_TOP.h, UI_TOP.w);
	wresize(UI_SEP.win, UI_SEP.h, UI_SEP.w);
	wresize(UI_BOT.win, UI_BOT.h, UI_BOT.w);

	wclear(UI_SEP.win);
	whline(UI_SEP.win, ACS_HLINE, UI_SEP.w);

	mvwin(UI_TOP.win, 0, 0);
	mvwin(UI_SEP.win, UI_TOP.h, 0);
	mvwin(UI_BOT.win, UI_TOP.h + UI_SEP.h, 0);

#if 0
	box(UI_TOP.win, 0, 0);
	box(UI_BOT.win, 0, 0);
#endif

	wmove(UI_TOP.win, UI_TOP.h - 1, 0);
	wmove(UI_BOT.win, UI_BOT.h - 1, 0);

	ui_refresh();
}

#if 0

void ui_handle_wench(int s) {
	signal(s, SIG_IGN);
	ui_init();
	signal(s, ui_handle_wench);
}

#endif

// Warning: reentrant
void ui_keypress(int c) {

	static char escape, meta;

	if(c == ERR || c >= KEY_MIN)
		return;

	int x, y;
	getyx(UI_TOP.win, y, x);

	if(escape) {
		if(meta) {
			switch(c) {
				case 'C': // Right arrow
					if(x < UI_TOP.w - 1) {
						x++;
						wmove(UI_TOP.win, y, x);
					}
					break;
				case 'D': // Left arrow
					if(x > 0) {
						x--;
						wmove(UI_TOP.win, y, x);
					}
					break;
				default:
					break;
			}
			meta = escape = 0;
		} else {
			switch(c) {
				case 91:
					meta = 1;
					break;
				default:
					break;
			}
		}

	} else {
		switch(c) {
			case 27: // Escape
				escape = 1;
				break;
			case KEY_BACKSPACE: // Bullshit
			case 127:
				if(x > 0) {
					wmove(UI_TOP.win, y, x - 1);
					wprintw(UI_TOP.win, " ");
					x--;
					wmove(UI_TOP.win, y, x);
				}
				break;

			default:
				//wprintw(UI_TOP.win, "%c = %d\n", c, c);
				wprintw(UI_TOP.win, "%c", c);
				break;
		}
	}
		
}
