#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

struct runlength_iter {
  char const *x;
};

void init_runlength_iter(struct runlength_iter *iter,
                         char const *x)
{
  iter->x = x;
}

char const *skip(char const *x, char c)
{
  assert(c != '\0');
  while (*x == c) x++;
  return x;
}

bool next_runlength(struct runlength_iter *iter,
                    char *character, int *length)
{
  if (*iter->x == '\0') return false;

  *character = *iter->x;
  char const *next = skip(iter->x, *character);
  *length = next - iter->x;
  iter->x = next;
  return true;
}

void cleanup_runlength_iter(struct runlength_iter *iter)
{
  // nothing to do here
}

int main(void)
{
  char const *x = "aabaabbbcbcc";

  struct runlength_iter iter;
  char c; int length;

  init_runlength_iter(&iter, x);
  while (next_runlength(&iter, &c, &length)) {
    printf("%c repeated %d times.\n", c, length);
  }
  cleanup_runlength_iter(&iter);

  return 0;
}
