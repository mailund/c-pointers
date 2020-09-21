#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int no_digits(int n)
{
  int digits = 0;
  for ( ; n ; n /= 10)
    digits++;
  return digits;
}

void neg_int_to_string(int n, char *s)
{
  static const char *digits = "0123456789";
  for ( ; n ; n /= 10) {
    *s-- = digits[-(n % 10)];
  }
}

void int_to_string(int n, char *s)
{
  char *x = s;
  if (n == 0) {
    s[0] = '0'; s[1] = '\0';
    return;
  }

  if (n < 0) *s++ = '-';
  if (n > 0) n = -n;
  s += no_digits(n);
  *s-- = '\0';
  neg_int_to_string(n, s);
}

char *neg_int_to_string_(int n, char *s)
{
  static const char *digits = "0123456789";

  if (n <= -10) {
    s = neg_int_to_string_(n / 10, s);
  }
  *s = digits[-(n % 10)]; // we can represtent -(n % 10)
  return s + 1;
}

void int_to_string_(int n, char *s)
{
  if (n == 0) {
    s[0] = '0'; s[1] = '\0';
    return;
  }

  if (n < 0) *s++ = '-';
  if (n > 0) n = -n;
  s = neg_int_to_string_(n, s);
  *s = '\0';
}

void neg_int_to_string__(int n, char **s)
{
  static const char *digits = "0123456789";
  if (n <= -10) {
    neg_int_to_string__(n / 10, s);
  }
  *(*s)++ = digits[-(n % 10)];
}

void int_to_string__(int n, char *s)
{
  if (n == 0) {
    s[0] = '0'; s[1] = '\0';
    return;
  }

  if (n < 0) *s++ = '-';
  if (n > 0) n = -n;
  neg_int_to_string__(n, &s);
  *s = '\0';
}

int main(int argc, char **argv)
{
  int n = 11;
  int digits = log10(n) + 1;
  char buf[digits], buf2[digits], buf3[digits];

  for (int i = -n; i < n; i++) {
    int_to_string(i, buf);
    int_to_string_(i, buf2);
    int_to_string__(i, buf3);
    printf("%d = '%s' / '%s' / '%s'\n", i, buf, buf2, buf3);
  }

  return EXIT_SUCCESS;
}
