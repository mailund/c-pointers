#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <assert.h>


char *skip_whitespace(char *x)
{
  while (*x && isspace(*x))
    x++;
  return x;
}

char *next_whitespace(char *x)
{
  while (*x && !isspace(*x))
    x++;
  return x;
}

struct word_iter {
  char *next_word;
};

void init_word_iter(struct word_iter *iter,
                    char *string)
{
  iter->next_word = skip_whitespace(string);
}

bool next_word(struct word_iter *iter,
              char **word)
{
  if (*iter->next_word == '\0') return false;
  *word = iter->next_word;
  iter->next_word =
    skip_whitespace(next_whitespace(iter->next_word));
  return true;
}

void dealloc_word_iter(struct word_iter *iter)
{
  // We don't need to clean anything up here
}

int main(int argc, char **argv)
{
  const char *words = "\tfoo  bar\n\tbaz qux\n";

  struct word_iter iter;
  init_word_iter(&iter, (char *)words);

  char *word;
  while (next_word(&iter, &word)) {
    printf("Current position: '%s'\n", word);
  }

  dealloc_word_iter(&iter);

  return EXIT_SUCCESS;
}
