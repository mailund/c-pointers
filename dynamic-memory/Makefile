
string_tests = string-test

rest = malloc

all: $(string_tests) $(rest)

$(string_tests): %: %.o strings.o strings.h
	cc $< strings.o -o $@

$(rest): %: %.c
	cc -Wall -pedantic $< -o $@

%.o: %.c
	cc -c -Wall -pedantic $< -o $@
