
tests = singly-linked-lists \
        head-singly-linked-lists \
				pointer-singly-linked-lists \
				doubly-linked-lists

all: $(tests)

$(tests): %: %.c
	cc -Wall -pedantic $< -o $@
