#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

int no_digits(int n)
{
  assert(n > 0);
  return (int)log10(n) + 1;
}

void pos_int_to_string(int n, char *s)
{
  assert(n > 0);
  static const char *digits = "0123456789";
  s += no_digits(n);
  *s-- = '\0';
  while (n) {
    *s-- = digits[n % 10];
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


void pos_int_to_string_index(int n, char *s)
{
  assert(n > 0);
  static const char *digits = "0123456789";
  int i = no_digits(n) + 1;
  s[--i] = '\0';
  while (n) {
    s[--i] = digits[n % 10];
    n /= 10;
  }
}

void int_to_string_index(int n, char *s)
{
  if (n == 0) {
    s[0] = '0'; s[1] = '\0';
  } else if (n < 0) {
    s[0] = '-';
    pos_int_to_string_index(-n, s + 1);
  } else {
    pos_int_to_string_index(n, s);
  }
}

void pos_int_to_string_offset(int n, char *s)
{
  assert(n > 0);
  static const char *digits = "0123456789";
  int i = no_digits(n) + 1;
  *(s + --i) = '\0';
  while (n) {
    *(s + --i) = digits[n % 10];
    n /= 10;
  }
}

void int_to_string_offset(int n, char *s)
{
  if (n == 0) {
    s[0] = '0'; s[1] = '\0';
  } else if (n < 0) {
    s[0] = '-';
    pos_int_to_string_offset(-n, s + 1);
  } else {
    pos_int_to_string_offset(n, s);
  }
}


int main(int argc, char **argv)
{
  int n = 11;
  char buf[no_digits(n) + 1 + 1]; // +1 for '-' and +1 for '\0'
  char buf2[no_digits(n) + 1 + 1]; // +1 for '-' and +1 for '\0'
  char buf3[no_digits(n) + 1 + 1]; // +1 for '-' and +1 for '\0'

  for (int i = -n; i < n; i++) {
    int_to_string(i, buf);
    int_to_string_index(i, buf2);
    int_to_string_offset(i, buf3);
    printf("%d = '%s' / '%s' / '%s'\n", i, buf, buf2, buf3);
  }

  return EXIT_SUCCESS;
}
