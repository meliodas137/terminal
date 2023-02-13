CC=gcc
CFLAGS=-g -pedantic -std=gnu17 -Wall -Werror -Wextra

.PHONY: all
all: nyush

nyush: nyush.o cmdutils.o

nyush.o: nyush.c cmdutils.o

cmdutils.o: cmdutils.c cmdutils.h

.PHONY: clean
clean:
	rm -f *.o nyush
