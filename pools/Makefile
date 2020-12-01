
tests = node_pool node_pool-resize node_pool-free generic-pool

all: $(tests)

$(tests): %: %.c
	cc -Wall -pedantic -O3 $< -o $@
