CC=gcc
CFLAGS=-O2 -std=c89 -W -Werror -Wall -Wextra -pedantic -I. 
LFLAGS=-lm
all:
	$(CC) $(CFLAGS) main.c i2c.c lis3dh.c -o lis3dh $(LFLAGS)

clean:
	rm -rf lis3dh
