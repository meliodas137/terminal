CC=gcc
CFLAGS=-g -pedantic -std=gnu17 -Wall -Werror -Wextra

.PHONY: all
all: terminal

terminal: terminal.o

terminal.o: terminal.c

.PHONY: clean
clean:
	rm -f *.o nyuc
