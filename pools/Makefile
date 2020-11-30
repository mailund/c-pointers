
tests = node_pool node_pool-resize

all: $(tests)

$(tests): %: %.c
	cc -Wall -pedantic -O3 $< -o $@
