CC=gcc
CFLAGS=-Wall -g -ansi -pedantic
LDFLAGS=-lpcre -pthread

OUTPUT=glife
SRC=config.c game.c main.c mem.c

DOXYGEN=doxygen
DOXYCONF=doxygen.conf
DOXYDIR=${PWD}/doc

all:
	$(CC) $(CFLAGS) $(LDFLAGS) $(SRC) -o $(OUTPUT)

doc:
	$(DOXYGEN) $(DOXYCONF)

clean:
	rm -fr $(OUTPUT) $(DOXYDIR)
