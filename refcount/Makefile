
tests = lists extension-lists refcount

all: $(tests)

$(tests): %: %.c
	cc -Wall -pedantic -O3 $< -o $@
