
tests = simple reference reference-refactored morris iterative
CFLAGS = -Wall -pedantic

all: $(tests)

$(tests): %: %.c
	cc $(CFLAGS) $< -o $@
