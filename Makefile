CC=gcc
CFLAGS = -g -O0 -Wall -Wextra -Wpedantic -Wshadow -Wformat=2 -Wfloat-equal \
         -Wconversion -Wcast-align -Wpointer-arith -Wstrict-overflow=5 \
         -Wwrite-strings -Wcast-qual -Wswitch-default -Wswitch-enum \
         -Wunreachable-code -Wstrict-prototypes -Wmissing-prototypes \
         -Wmissing-declarations -Wredundant-decls -Wold-style-definition \
         -Winline -Wlogical-op -Wjump-misses-init -Wdouble-promotion \
         -Wformat-overflow=2 -Wformat-signedness -Wmissing-include-dirs \
         -Wnull-dereference -Wstack-usage=1024 -Wpacked -Woverlength-strings \
         -Wvla -Walloc-zero -Wduplicated-cond -Wduplicated-branches -Wrestrict \
         -fanalyzer \
		 -I. -std=c89

CFILES = $(wildcard *.c)
OBJECTS = $(CFILES:.c=.o)
BIN = lis3dh

all: $(BIN)

$(BIN): $(OBJECTS)
	@echo ">>> $@"
	@$(CC) $(CFLAGS) $^ -o $@

%.o:%.c
	@echo "cc $<"
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJECTS)