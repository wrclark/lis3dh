CC=gcc
CFLAGS=-O2 -std=c99 -W -Werror -Wall -Wextra -I. 
all:
	$(CC) $(CFLAGS) main.c i2c.c lis3dh.c -o lis3dh -lm

clean:
	rm -rf lis3dh