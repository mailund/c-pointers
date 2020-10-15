
tests = malloc calloc strings dynarray gapbuf

all: $(tests)

$(tests): %: %.c
	cc -Wall -pedantic $< -o $@
