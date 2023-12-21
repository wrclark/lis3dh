CC=gcc
CFLAGS=-O2 -std=gnu99 -W -Werror -Wall -Wextra -I.
all:
	$(CC) $(CFLAGS) main.c lis3dh.c -o main