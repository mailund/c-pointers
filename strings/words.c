#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>

char *skip_word(char *x)
{
  while (*x && isalpha(*x))
    x++;
  return x;
}

char *find_word(char *x)
{
  while (*x && !isalpha(*x))
    x++;
  return x;
}

int main(int argc, char **argv)
{
  const char *words = "\tfoo!  bar\n\tbaz qux\n";

  char *x = find_word((char *)words);
  while (*x) {
    printf("Current position: '%s'\n", x);
    x = find_word(skip_word(x));
  }

  return EXIT_SUCCESS;
}
