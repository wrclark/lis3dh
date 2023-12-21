CC=gcc
CFLAGS=-O2 -std=gnu99 -W -Werror -Wall -Wextra -I.
all:
	$(CC) $(CFLAGS) main.c i2c.c lis3dh.c -o main

clean:
	rm -rf main