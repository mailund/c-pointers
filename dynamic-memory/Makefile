
tests = malloc calloc realloc \
        strings dynarray gapbuf

all: $(tests)

$(tests): %: %.c
	cc -Wall -pedantic $< -o $@
