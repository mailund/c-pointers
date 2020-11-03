
tests = simple reference reference-refactored iterative \
			  explicit-stack morris 
CFLAGS = -Wall -pedantic

all: $(tests)

$(tests): %: %.c
	cc $(CFLAGS) $< -o $@
