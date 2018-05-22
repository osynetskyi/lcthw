CFLAGS=-Wall -g -DNDEBUG

all: ex1 ex3

ex22.o: ex22.h

ex22_main: ex22.o

.PHONY: clean
clean:
	rm -f ex1 ex3
