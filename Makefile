CC=gcc
CFLAGS=-O2 -std=c89 -W -Werror -Wall -Wextra -pedantic -I. 
LFLAGS=-lm
CFILES=$(wildcard ./*.c)
all:
	$(CC) $(CFLAGS) $(CFILES) -o lis3dh $(LFLAGS)

clean:
	rm -rf lis3dh
