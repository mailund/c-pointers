
tests = higher-order-functions \
	gen gen-tagged
CFLAGS = -Wall -pedantic

all: $(tests)

higher-order-functions: %: %.c list.o
	cc $(CFLAGS) $< list.o -o $@

gen: %: %.c
	cc $(CFLAGS) -O3 $< -o $@

list.o: list.c list.h
