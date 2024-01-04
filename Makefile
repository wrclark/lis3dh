CC=gcc
CFLAGS=-O0 -g3 -ggdb -std=c89 -W -Werror -Wall -Wextra -Wpedantic -pedantic-errors -Wformat-signedness -I.
CFLAGS+=-Wlogical-op -Wmissing-declarations -Wswitch-default -Wundef -Wformat=2
LFLAGS=-lm
CFILES=$(wildcard *.c)
BINFILE=lis3dh

all:
	$(CC) $(CFLAGS) $(CFILES) -o $(BINFILE) $(LFLAGS)

clean:
	rm -rf $(BINFILE)
