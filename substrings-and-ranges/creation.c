#include <stdio.h>
#include <string.h>
#include "substr.h"

int main(void)
{
  do {
    char s[] = "hello, world";
    substr ss = as_substr(s);
    printf("%zu %zu\n", strlen(s), substr_len(ss));

    printf("slicing \""); print_substr(ss); printf("\"");
    ss = slice(s, 0, 5);
    printf(" gives us \""); print_substr(ss); printf("\"\n");
    printf("%zu %zu\n", strlen(s), substr_len(ss));

    char buf[substr_len(ss) + 1];
    substr_to_buf(buf, ss);
    printf("%s\n", buf);

    char c = zero_term(ss);
    printf("%s\n", ss.begin);
    restore_term(ss, c);
  } while(0);

  return 0;
}
