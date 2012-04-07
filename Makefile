#gmake Makefile
EXE = cheaptalk
SHELL = bash

CFLAGS = -Wall
LFLAGS = -lncurses 
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
