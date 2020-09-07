#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

int main(int argc, char **argv)
{
  const char *string = "hello, world!\n";
  const char *cp = string;
  int n = strlen(string);
  for (int i = 0; i < n; i++) {
    assert(string[i] == cp[i]);
    assert(string + i == cp + i);
    assert(*(string + i) == *(cp + i));
  }

  return EXIT_SUCCESS;
}
