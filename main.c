/*
 * Goal: Very simple p2p realtime chat which visually resembles 'talk'
 * - No daemon/multiuser/tty/user stuff, just p2p
 *
 * - Realtime character-by-character tx/rx (no utf-8 support, eg won't use widechars)
 * - Allow resizing of terminal
 * - Use half the horizontal height of a terminal for each user
 * - Allow ANSI codes
 * - Use SDL_Net for networking (easily replaced with othher interface)
*/

#include "main.h"

#define VERSION "0.1"

void usage(char *me) {
	printf("Cheaptalk v%s by gammy\n"
	       "Usage: %s [options]\n"
	       "To wait for a friend  : %s --listen\n"
	       "To connect to a friend: %s --connect <friends hostname>\n\n"
	       "-l  --listen                Wait for someone to connect to you\n"
	       "-c  --connect <host>        Connect to <host>\n"
	       "-p  --port <port>           Listen or connect using port <port>\n"
	       "-h  --help                  This help\n"
	       "-V  --version               Display version\n\n"
	       "This is free software; see the source for copying conditions. "
	       "There is NO\nwarranty; not even for MERCHANTABILITY or FITNESS "
	       "FOR A PARTICULAR PURPOSE.\n\n",
	       VERSION, 
	       me, me, me);
}

int main(int argc, char *argv[]){

	static struct option const long_options[] = {
		{"listen",     no_argument,       NULL, 'l'},
		{"connect",    required_argument, NULL, 'c'},
		{"port",       required_argument, NULL, 'd'},
		{"version",    no_argument,       NULL, 'V'},
		{"help",       no_argument,       NULL, 'h'},
		{NULL,         0,                 NULL,  0}
	};

	int mode           = MODE_INVALID;
	char *port         = DEFAULT_PORT;
	char *host         = NULL;

	unsigned int i;

	while((i = getopt_long(argc, argv, "lc:p:Vh", long_options, NULL)) != -1){
		switch(i) {
			case 'l':
				mode = MODE_SERVER;
				break;
			case 'c':
				mode = MODE_CLIENT;
				host = optarg;
				break;
			case 'p':
				port = optarg;
				break;
			case 'h':
				usage(basename(argv[0]));
				return(EXIT_SUCCESS);
			case 'V':
				printf("Cheaptalk v%s by gammy\n", VERSION);
				return(EXIT_SUCCESS);
			default:
				break;
		}
	}

	if(mode == MODE_INVALID) {
		fprintf(stderr, "\nError: You need to --listen or --connect!\n\n");
		usage(basename(argv[0]));
		return(EXIT_FAILURE);
	}

	if(! net_init())
		return(EXIT_FAILURE);

#ifdef DEBUG
	if(mode == MODE_CLIENT)
		printf("Host: %s\n", host);
	printf("Port: %s\n", port);
	printf("Mode: %s\n", mode == MODE_CLIENT ? "Client" : "Server");
#endif

	if(! net_begin(mode, host, port))
		return(EXIT_FAILURE);

	ui_nullify();
	initscr();
	cbreak();
	noecho();
	nodelay(stdscr, TRUE);
	curs_set(2);
	use_default_colors(); // Ensure user-set bg and/or fg-colors are respected
	start_color();        // We want to allow passing ANSI color, so we need color
	ui_resized(); 
	ui_init();
	nodelay(UI_TOP.win, TRUE);

	char busy = 1; // FIXME maybe make global later so we can catch interrupts and shut down
	int counter = 0;

	while(busy) {

		//if(ui_resized())
		//	ui_resize();

		int c = wgetch(UI_TOP.win);
		//int c = getch();

		ui_keypress(&UI_TOP, c);
		if(c != ERR)
			net_send(mode, &c);

		if(net_recv(mode, &c))
			ui_keypress(&UI_BOT, c);

		mvwprintw(UI_SEP.win, 0, (UI_SEP.w / 2) - 4, "%d", counter++);

		//mvwprintw(UI_TOP.win, 0, 0, "%dx%d  ", UI_MAIN.w, UI_MAIN.h);
		
		//wprintw(UI_TOP.win, "Test\n");
		//wprintw(UI_BOT.win, "Fist");
		//wprintw(UI_BOT.win, "Fist\n");

		ui_refresh();

		usleep(5000);
	}

	endwin();
	net_deinit();

	return(EXIT_SUCCESS);
}
