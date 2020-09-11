#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

int no_digits(int n)
{
  assert(n > 0);
  return log10(n) + 1;
}

void pos_int_to_string(int n, char *s)
{
  assert(n > 0);
  static const char *digits = "0123456789";
  s += no_digits(n) + 1;
  *(--s) = '\0';
  while (n) {
    *(--s) = digits[n % 10];
    n /= 10;
  }
}

void int_to_string(int n, char *s)
{
  if (n == 0) {
    s[0] = '0'; s[1] = '\0';
  } else if (n < 0) {
    s[0] = '-';
    pos_int_to_string(-n, s + 1);
  } else {
    pos_int_to_string(n, s);
  }
}

int main(int argc, char **argv)
{
  int n = 11;
  char buf[no_digits(n) + 1 + 1]; // +1 for '-' and +1 for '\0'

  for (int i = -n; i < n; i++) {
    int_to_string(i, buf);
    printf("%d = '%s'\n", i, buf);
  }

  return EXIT_SUCCESS;
}
