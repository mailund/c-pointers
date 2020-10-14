
tests = malloc strings

all: $(tests)

$(tests): %: %.c
	cc -Wall -pedantic $< -o $@
