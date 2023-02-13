CC=gcc
CFLAGS=-g -pedantic -std=gnu17 -Wall -Werror -Wextra

.PHONY: all
all: terminal

terminal: terminal.o cmdutils.o

terminal.o: terminal.c cmdutils.o

cmdutils.o: cmdutils.c cmdutils.h

.PHONY: clean
clean:
	rm -f *.o nyush
