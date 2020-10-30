
tests = refcount list list-macros

all: $(tests)

$(tests): %: %.c
	cc -Wall -pedantic -O3 $< -o $@
