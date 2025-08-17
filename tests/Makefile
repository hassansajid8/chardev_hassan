CC = gcc
CFLAGS = -Wall -Wextra -O2

SRC := $(wildcard *.c)
PROGS := $(SRC:.c=)

all: $(PROGS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(PROGS) *.o
