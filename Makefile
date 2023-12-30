CC=gcc
CFLAGS=-O0 -g3 -ggdb -std=c89 -W -Werror -Wall -Wextra -Wpedantic -pedantic-errors -Wformat-signedness -I.
CFLAGS+=-Wlogical-op -Wmissing-declarations -Wswitch-default -Wundef -Wformat=2 -lasan
CFLAGS+=-fsanitize=pointer-overflow,undefined,shift,shift-exponent,shift-base,integer-divide-by-zero,null,signed-integer-overflow,bounds
LFLAGS=-lm
CFILES=$(wildcard *.c)
all:
	$(CC) $(CFLAGS) $(CFILES) -o lis3dh $(LFLAGS)

clean:
	rm -rf lis3dh
