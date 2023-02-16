CC=gcc
CFLAGS=-g -pedantic -std=gnu17 -Wall -Werror -Wextra

.PHONY: all
all: nyush

nyush: nyush.o executor.o cmdutils.o

nyush.o: nyush.c executor.o cmdutils.o

executor.o: executor.c executor.h cmdutils.o

cmdutils.o: cmdutils.c cmdutils.h

.PHONY: clean
clean:
	rm -f *.o nyush
