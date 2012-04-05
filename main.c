/*
 * Goal: Very simple p2p realtime chat which visually resembles 'talk'
 * - No daemon/multiuser/tty/user stuff, just simple p2p
 *
 * - Use half the horizontal height of a terminal for each user
 * - Realtime character-by-character tx/rx (no utf-8 support)
 * - Allow resizing of terminal
 * - Allow ANSI codes
 * - Use SDL_Net for sockets
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
	unsigned long port = DEFAULT_PORT;

	unsigned int i;

	while((i = getopt_long(argc, argv, "lc:p:Vh", long_options, NULL)) != -1){
		switch(i){
		case 'l':
			mode = MODE_SERVER;
			break;
		case 'c':
			mode = MODE_CLIENT;
			break;
		case 'p':
			port = atoi(optarg);
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

	ui_nullify();
	initscr();
	cbreak();
	noecho();
	nodelay(stdscr, TRUE);
	start_color();

	ui_resized();
	ui_init();

	while(getch() != 27) {

		if(ui_resized())
			ui_init();

		usleep(50000);
	}

	return(EXIT_SUCCESS);
}
