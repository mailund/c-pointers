
tests = higher-order-functions gen
CFLAGS = -Wall -pedantic

all: $(tests)

higher-order-functions: %: %.c list.o
	cc $(CFLAGS) $< list.o -o $@

list.o: list.c list.h
