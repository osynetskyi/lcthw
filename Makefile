CFLAGS=-Wall -g -DNDEBUG
LIBS=-lm -lbsd -Lliblcthw/build -llcthw

all: ex1 ex3

ex22.o: ex22.h

ex22_main: ex22.o

ex43_1: CFLAGS+=$(LIBS)
ex43_2: CFLAGS+=$(LIBS)

.PHONY: clean
clean:
	rm -f ex1 ex3
