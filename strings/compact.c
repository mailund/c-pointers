#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

char *find_word(char *x)
{
  while (*x && !isalpha(*x))
    x++;
  return x;
}

void copy_word(char **from, char **to)
{
  while (isalpha(**from)) {
    *(*to)++ = *(*from)++;
  }
}

void copy_words(char *from, char *to)
{
  from = find_word(from);
  while (*from) {
    copy_word(&from, &to);
    from = find_word(from);
    if (*from) *to++ = ' ';
  }
  *to = '\0';
}

void compact_words(char *s)
{
  copy_words(s, s);
}

int main(int argc, char **argv)
{
  // We modify the string we wordize, so we
  // cannot use a literate string. Those are immutable
  // and if we try, the program will probably crash.
  const char *orig_string = "\tfoo!  bar\n\tbaz qux\n";
  char string[strlen(orig_string) + 1];

  copy_words((char *)orig_string, string);
  printf("'%s'\n", string);

  strcpy(string, orig_string);

  compact_words(string);
  printf("'%s'\n", string);

  return EXIT_SUCCESS;
}