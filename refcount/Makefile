
tests = singly-linked-lists-1 \
				singly-linked-lists-2

all: $(tests)

$(tests): %: %.c
	cc -Wall -pedantic -O3 $< -o $@
