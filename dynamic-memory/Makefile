
tests = malloc strings gapbuf

all: $(tests)

$(tests): %: %.c
	cc -Wall -pedantic $< -o $@
