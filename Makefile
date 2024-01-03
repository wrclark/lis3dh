CC=gcc
CFLAGS=-O0 -g3 -ggdb -std=c89 -W -Werror -Wall -Wextra -Wpedantic -pedantic-errors -Wformat-signedness -I.
CFLAGS+=-Wlogical-op -Wmissing-declarations -Wswitch-default -Wundef -Wformat=2 -lasan -lm
CFLAGS+=-fsanitize=pointer-overflow,undefined,shift,shift-exponent,shift-base,integer-divide-by-zero,null,signed-integer-overflow,bounds
CFILES=$(wildcard *.c)
BINFILE=lis3dh

all:
	$(CC) $(CFLAGS) $(CFILES) -o $(BINFILE)

clean:
	rm -rf $(BINFILE)
