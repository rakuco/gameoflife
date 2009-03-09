CC=gcc
CFLAGS=-Wall -g -ansi -pedantic
LDFLAGS=-lpcre -pthread

OUTPUT=glife
SRC=config.c game.c main.c mem.c

all:
	$(CC) $(CFLAGS) $(LDFLAGS) $(SRC) -o $(OUTPUT)

clean:
	rm -f $(OUTPUT)
