CC=gcc
CFLAGS=-Wall -g
LDFLAGS=-lpthread

OUTPUT=glife
SRC=config.c main.c

all:
	$(CC) $(CFLAGS) $(LDFLAGS) $(SRC) -o $(OUTPUT)

clean:
	rm -f $(OUTPUT)
