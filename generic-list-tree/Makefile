
lists = casting offset
tests = $(lists)
CFLAGS = -Wall -pedantic

all: $(tests)

$(lists): %: %.c list.o
	cc $(CFLAGS) $< list.o -o $@

list.o: list.c list.h
