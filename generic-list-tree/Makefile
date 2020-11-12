
tests = list-experiments
CFLAGS = -Wall -pedantic

all: $(tests)

list-experiments: %: %.c list.o
	cc $(CFLAGS) $< list.o -o $@

list.o: list.c list.h
