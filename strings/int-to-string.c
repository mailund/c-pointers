#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <strings.h>

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

void reverse_string(char *s)
{
  char *left = s;
  char *right = s + strlen(s) - 1;
  for ( ; left < right; left++, right--) {
    char c = *left;
    *left = *right;
    *right = c;
  }
}

void neg_int_to_string_rev(int n, char *s)
{
  // we need to remember the beginning for reversal
  char *front = s;
  static const char *digits = "0123456789";
  for ( ; n ; n /= 10) {
    *s++ = digits[-(n % 10)];
  }
  reverse_string(front);
}

void int_to_string_rev(int n, char *s)
{
  if (n == 0) {
    s[0] = '0'; s[1] = '\0';
    return;
  }

  if (n < 0) *s++ = '-';
  if (n > 0) n = -n;
  neg_int_to_string_rev(n, s);
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
  char buf[digits], buf2[digits], buf3[digits], buf4[digits];

  for (int i = -n; i < n; i++) {
    int_to_string(i, buf);
    int_to_string_rev(i, buf2);
    int_to_string_(i, buf3);
    int_to_string__(i, buf4);
    printf("%d = '%s' / '%s' / '%s' / '%s'\n", i, buf, buf2, buf3, buf4);
  }

  return EXIT_SUCCESS;
}
