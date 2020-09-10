#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <assert.h>

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

struct word_iter {
  char *x;
  char replaced;
};

void init_word_iter(struct word_iter *iter,
                    char *string)
{
  iter->x = string;
  iter->replaced = *string;
}

bool next_word(struct word_iter *iter,
                char **word)
{
  *iter->x = iter->replaced;
  *word = find_word(iter->x);
  if (**word == '\0') return false;
  iter->x = skip_word(*word);
  iter->replaced = *iter->x;
  *iter->x = '\0';
  return true;
}

void dealloc_word_iter(struct word_iter *iter)
{
  *iter->x = iter->replaced;
}

int main(int argc, char **argv)
{
  // We modify the string we wordize, so we
  // cannot use a literate string. Those are immutable
  // and if we try, the program will probably crash.
  const char *orig_string = "\tfoo!  bar\n\tbaz qux\n";
  char string[strlen(orig_string) + 1];
  strcpy(string, orig_string);

  // Setup iterator
  struct word_iter iter;
  init_word_iter(&iter, string);

  // Iterate
  char *word;
  while (next_word(&iter, &word)) {
    printf("word: '%s'\n", word);
  }

  // Cleanup
  cleanup_word_iter(&iter);

  // We have restored the string to its original shape
  assert(strcmp(string, orig_string) == 0);

  return EXIT_SUCCESS;
}
