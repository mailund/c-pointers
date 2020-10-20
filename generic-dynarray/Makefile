
tests = void-array buf-array code-generation inline

all: $(tests)

$(tests): %: %.c
	cc -std=c11 -Wall -pedantic $< -o $@
