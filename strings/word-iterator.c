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

struct word_iter {
  char *next_word;
};

void init_word_iter(struct word_iter *iter,
                    char *string)
{
  iter->next_word = find_word(string);
}

bool next_word(struct word_iter *iter,
               char **word)
{
  if (*iter->next_word == '\0') return false;
  *word = iter->next_word;
  iter->next_word =
    find_word(skip_word(iter->next_word));
  return true;
}

void cleanup_word_iter(struct word_iter *iter)
{
  // We don't need to clean anything up here
}

int main(void)
{
  char const *words = "\tfoo!  bar\n\tbaz qux\n";

  struct word_iter iter;
  init_word_iter(&iter, (char *)words);

  char *word;
  while (next_word(&iter, &word)) {
    printf("Current position: '%s'\n", word);
  }

  cleanup_word_iter(&iter);

  return 0;
}
