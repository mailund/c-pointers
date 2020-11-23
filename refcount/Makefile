
tests = lists

all: $(tests)

$(tests): %: %.c
	cc -Wall -pedantic -O3 $< -o $@
