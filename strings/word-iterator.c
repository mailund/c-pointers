#include <stdio.h>
#include <ctype.h>

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

// NULL pointer instead of pointer to zero terminal
#define NULLIFY(x) ((*x) ? x : 0)
#define WORD_ITER_END 0

char *first_word(char *x)
{
  return NULLIFY(find_word(x));
}

char *next_word(char *x)
{
  return NULLIFY(find_word(skip_word(x)));
}

int main(void)
{
  char const *words = "\tfoo!  bar\n\tbaz qux\n";

  for (char *iter = first_word((char *)words);
       iter != WORD_ITER_END;
       iter = next_word(iter)) {
    printf("Current position: '%s'\n", iter);
  }

  return 0;
}
