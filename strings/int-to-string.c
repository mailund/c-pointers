#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

void int_to_string(int n, char *s)
{
  assert(n > 0);
  static const char *digits = "0123456789";
  s += (int)log10(n) + 1;
  while (n) {
    *(--s) = digits[n % 10];
    n /= 10;
  }
}

int main(int argc, char **argv)
{
  int n = 101;
  char buf[(int)log10(n) + 1 + 1]; // extra +1 for '\0'

  for (int i = 1; i < 101; i++) {
    int_to_string(i, buf);
    printf("%d = '%s'\n", i, buf);
  }

  return EXIT_SUCCESS;
}
