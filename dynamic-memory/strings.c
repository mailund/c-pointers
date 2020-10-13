#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "strings.h"

char const *slice(char const *x, int i, int j)
{
  assert(i <= j);
  size_t len = j - i;

  char *new_buf = malloc(len + 1);
  if (!new_buf) return 0;

  strncpy(new_buf, x + i, len);
  new_buf[len] = '\0';
  return new_buf;
}

char *replace_string(char const *x, int i, int j, char const *y)
{
  size_t xlen = strlen(x);
  size_t ylen = strlen(y);
  size_t len = xlen - (j - i) + ylen;

  char *new_buf = malloc(len + 1);
  if (!new_buf) return 0;

  strncpy(new_buf, x, i);
  strcpy(new_buf + i, y);
  strcpy(new_buf + i + ylen, x + j);

  return new_buf;
}

char *insert_string(char const *x, int i, char const *y)
{
  return replace_string(x, i, i, y);
}

char *delete_string(char const *x, int i, int j)
{
  return replace_string(x, i, j, "");
}

// copies up to, but not including, the zero-terminal
#define string_copy(dst, src) \
{ while (*(src)) *(dst)++ = *(src)++; }

char *join_strings(char const *sep, int n, char const *strings[n])
{
  if (n == 0) {
    // We could consider this an error and return NULL.
    // We could also return an empty string, but we would have to
    // allocate it if we want the interface to this function to be
    // that all strings it returns are dynamically allocated.
    // I will go with the latter.
    char *x = malloc(1);
    if (x) *x = '\0';
    return x;
  }

  size_t no_strings = 0, string_len = 0;
  for (int i = 0; i < n; i++) {
    no_strings++;
    string_len += strlen(strings[i]);
  }
  size_t len = string_len + (no_strings - 1 ) * strlen(sep);
  char *new_buf = malloc(len + 1);
  char *dst = new_buf;
  char const *xsep = "";
  for (int i = 0; i < n; i++) {
    string_copy(dst, xsep);
    string_copy(dst, strings[i]);
    xsep = sep;
  }
  *dst = '\0';

  return new_buf;
}
