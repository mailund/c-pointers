#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "strings.h"

int main(void)
{
  char const *x = "foobarbaz";
  char *y = (char *)slice(x, 3, 6);
  printf("y is %s\n", y);
  assert(strcmp(y, "bar") == 0);
  free(y);

  y = replace_string(x, 3, 6, "");
  printf("y is %s\n", y);
  assert(strcmp(y, "foobaz") == 0);
  free(y);

  y = replace_string(x, 3, 6, "quxqax");
  printf("y is %s\n", y);
  assert(strcmp(y, "fooquxqaxbaz") == 0);
  free(y);

  y = insert_string(x, 0, "XX");
  printf("y is %s\n", y);
  assert(strcmp(y, "XXfoobarbaz") == 0);
  free(y);

  y = delete_string(x, 0, 3);
  printf("y is %s\n", y);
  assert(strcmp(y, "barbaz") == 0);
  free(y);

  char const *strings[] = {
    "foo", "bar", "baz"
  };
  char *z = join_strings(":", sizeof strings / sizeof *strings, strings);
  printf("z = %s\n", z);
  free(z);

  x = "foobarbazbax";
  z = replace_all_occurrences(x, "ba", "");
  printf("z == %s\n", z);
  free(z);

  z = replace_all_occurrences(x, "a", "OOOO");
  printf("z == %s\n", z);
  free(z);

  z = replace_all_occurrences(x, "123", "OOOO");
  printf("z == %s\n", z);
  free(z);

  return 0;
}
