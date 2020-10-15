
tests = malloc calloc strings gapbuf

all: $(tests)

$(tests): %: %.c
	cc -Wall -pedantic $< -o $@
