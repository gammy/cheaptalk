#gmake Makefile
EXE = cheaptalk
SHELL = bash

#CFLAGS = -I../../src/ -L../../src/ -std=gnu99 -ggdb3 -Wall -lgam $(GAMFLAGS) `sdl-config --cflags`
#CFLAGS = -std=gnu99 -ggdb3 -Wall `sdl-config --cflags`
CFLAGS = -ggdb3 -Wall `sdl-config --cflags`
LFLAGS = -lncurses `sdl-config --libs` -lSDL_net

CC     = gcc
OBJS   = $(patsubst %.c, %.o, $(wildcard ./*.c))

all: $(EXE)

$(EXE): $(OBJS)
	@echo -e "\e[4;1m$@\e[0m"
	$(CC) $(CFLAGS) $(LFLAGS) -o $(EXE) $(OBJS)
	@echo
	@echo "Success: $(EXE)"

%.o: %.c
	@echo -e "\e[4;1m$@\e[0m"
	$(CC) $(CFLAGS) -c -o $@ $<
	@echo

clean:
	-rm -f $(OBJS) $(EXE)
