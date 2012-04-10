/*
 * Goal: Very simple p2p realtime chat which visually resembles 'talk'
 *
 * Original goals: 
 * - No daemon/multiuser/tty/user stuff, just p2p
 * [X] Realtime character-by-character tx/rx (no utf-8 support, eg won't use widechars)
 * [X] Allow resizing of terminal
 *   [X] Use half the horizontal height of a terminal for each user
 *   [ ] Allow ANSI codes
 *     [ ] Translate ANSI text attributes to ncurses 
 *         (but why bother? How would you 'paste' escape sequences to the buffer?)
 *
 * Missing (which talk can do)
 * - Scroll in buffer (ESC p/n for top, ctrl+p/n for bottom)
 *
 * Added:
 * - Simple color selection with ESC 0-6 or ESC r,g,b
 * - Move freely (with arrow keys) within the buffer 
 *   (note that hitting return will clear the line from the point of the cursor)
 *
 */

#include "main.h"

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

void signal_handle(int sig) {
	endwin();
	fprintf(stderr, "Caught signal %d\n", sig);
	busy = 0;
	//net_finish();
	//exit(EXIT_SUCCESS);
}

void signal_install(void) {

	struct sigaction sig_act_old, sig_act_new;

	sig_act_new.sa_handler = signal_handle;
	sigemptyset(&sig_act_new.sa_mask);

	sig_act_new.sa_flags = 0;

	sigaction(SIGINT, NULL, &sig_act_old);

	if(sig_act_old.sa_handler != SIG_IGN)
		sigaction(SIGINT, &sig_act_new, NULL);

	sigaction(SIGHUP, NULL, &sig_act_old);

	if(sig_act_old.sa_handler != SIG_IGN)
		sigaction(SIGHUP, &sig_act_new, NULL);

	sigaction(SIGTERM, NULL, &sig_act_old);

	if(sig_act_old.sa_handler != SIG_IGN)
		sigaction(SIGTERM, &sig_act_new, NULL);
	
	sigaction(SIGPIPE, NULL, &sig_act_old);

	if(sig_act_old.sa_handler != SIG_IGN)
		sigaction(SIGPIPE, &sig_act_new, NULL);
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
	
	signal_install();

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
	curs_set(1);
	use_default_colors(); // Ensure user-set bg and/or fg-colors are respected
	start_color();        // We want to eventually pass ANSI colors, so we need color
	init_pair(0, -1, -1);
	init_pair(1, COLOR_RED,     -1);
	init_pair(2, COLOR_GREEN,   -1);
	init_pair(3, COLOR_BLUE,    -1);
	init_pair(4, COLOR_YELLOW,  -1);
	init_pair(5, COLOR_CYAN,    -1);
	init_pair(6, COLOR_MAGENTA, -1);
	ui_resized(); 
	ui_init();
	nodelay(UI_TOP.win, TRUE);

#ifdef DEBUG
	int counter = 0;
#endif

	busy = 1;

	while(busy) {

		if(ui_resized())
			ui_resize();

		chtype c = wgetch(UI_TOP.win);

		if(c != ERR) {
			ui_keypress(&UI_TOP, c);
			net_send(&c);
			c = ERR; // Invalidate
			ui_refresh();
		}

		if(net_recv(&c)) {
			ui_keypress(&UI_BOT, c);
			ui_refresh();
		}

#ifdef DEBUG
		mvwprintw(UI_SEP.win, 0, (UI_SEP.w / 2) - 4, "%d", counter++);
#endif

		usleep(5000);
	}

	net_finish();
	endwin();

	if(busy == 2)
		printf("Other side closed the connection\n");

	return(EXIT_SUCCESS);
}
