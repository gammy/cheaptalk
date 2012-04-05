#ifndef MAIN_H
#define MAIN_H

//#define DEBUG

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // usleep
#include <ncurses.h>
#include <signal.h>
#include <libgen.h> // basename
#include <getopt.h>
#include <math.h>   // ceil
#include <SDL.h>

#include <assert.h>

#include "ui.h"

#define DEFAULT_PORT 13512

enum {
	MODE_INVALID,
	MODE_SERVER,
	MODE_CLIENT
} modes;

void usage(char *me);
void curses_init(void);
void handle_resize(int s);

#endif
