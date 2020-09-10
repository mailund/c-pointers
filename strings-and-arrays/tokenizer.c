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

struct tokenizer_iter {
  char *x;
  char replaced;
};

void init_tokenizer(struct tokenizer_iter *iter,
                    char *string)
{
  iter->x = string;
  iter->replaced = *string;
}

bool next_token(struct tokenizer_iter *iter,
                char **token)
{
  *iter->x = iter->replaced;
  *token = find_word(iter->x);
  if (**token == '\0') return false;
  iter->x = skip_word(*token);
  iter->replaced = *iter->x;
  *iter->x = '\0';
  return true;
}

void dealloc_tokenizer(struct tokenizer_iter *iter)
{
  *iter->x = iter->replaced;
}

int main(int argc, char **argv)
{
  // We modify the string we tokenize, so we
  // cannot use a literate string. Those are immutable
  // and if we try, the program will probably crash.
  const char *orig_string = "\tfoo!  bar\n\tbaz qux\n";
  char string[strlen(orig_string) + 1];
  strcpy(string, orig_string);

  struct tokenizer_iter iter;
  init_tokenizer(&iter, string);

  char *token;
  while (next_token(&iter, &token)) {
    printf("token: '%s'\n", token);
  }

  dealloc_tokenizer(&iter);

  // We have restored the string to its original shape
  assert(strcmp(string, orig_string) == 0);

  return EXIT_SUCCESS;
}
