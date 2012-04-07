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
	// This is in reverse order to ensure that the cursor is always last
	// set for the top window (ui_win[0] = UI_TOP)
	int i;
	for(i = 2; i >= 0; i--)
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

// Probably the most asinine part of this progeam.
void ui_keypress(screen_t *screen, int c) {

	static char escape, meta;

	if(c == ERR || c >= KEY_MIN)
		return;

	int x, y;
	getyx(screen->win, y, x);

	if(escape) {
		if(meta) {
			switch(c) {
				case 'A': // Up arrow
					if(y > 0)
						y--;
					break;
				case 'B': // Down arrow
					if(y < screen->h - 1)
						y++;
					break;
				case 'C': // Right arrow
					// FIXME I know this is off by one.
					//       It seems impossible to fix.
					if(x < screen->w - 2)
						x++;
					break;
				case 'D': // Left arrow
					if(x > 0)
						x--;
					break;
				default:
					break;
			}
			
			wmove(screen->win, y, x);
			meta = escape = 0;

		} else {
			switch(c) {
				case 91:
					meta = 1;
					break;
				case 'r': case 'R':
					wcolor_set(screen->win, 1, NULL);
					break;
				case 'g': case 'G':
					wcolor_set(screen->win, 2, NULL);
					break;
				case 'b': case 'B':
					wcolor_set(screen->win, 3, NULL);
					break;
				default:
					if(c > 47 && c <= 48+6)
						wcolor_set(screen->win, c - 48, NULL);
					break;
			}

			if(! meta)
				escape = 0;

		}

	} else {
		switch(c) { 
			case 27: // Escape
				escape = 1;
				return; // XXX
				break;
			case KEY_BACKSPACE: // Bullshit
			case 127:
				wmove(screen->win, y, x - 1);
				waddch(screen->win, ' ');
				x--;
				break;
			case 10: // Newline
			case 13:
				waddch(screen->win, c);
				x = 0;
				y++;
				break;
			default:
				//wprintw(screen->win, "%c = %d\n", c, c);
				waddch(screen->win, c);
				x++;
				break;
		}

		if(x < 0)
			x = 0;

		if(x >= screen->w - 1)  {
			waddch(screen->win, '\n'); 
			x = 0;
		}

		wmove(screen->win, y, x);
	}

#if 0
	mvwprintw(UI_BOT.win, 1, 1, "%dx%d ", x, y);
	wrefresh(UI_BOT.win);
#endif
		
}
