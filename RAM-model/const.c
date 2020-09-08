#include <stdlib.h>

int main(int argc, char **argv)
{
  const int i = 13;
  // i = 42; <- This is an error, i is const

  int j = 42;
  j = 13; // This is valid, j is not const

  // This declares a pointer to a constant.
  // It is okay if it points to i.
  const int *ip = &i; // This is fine

  // We can change the pointer (just not what it points to)
  ip = &j; // This is fine.
  // j isn't constant, but we can still point to it. We are
  // just not allowed to change what we point to.
  // *ip = 42; <- This is an error

  // This declares a const pointer to a non-const integer
  int * const jp = &j;
  // We could not point jp to i, because i is const and
  // *jp is not. It is fine to point it to non-const j, though.

  // We can change what jp points to, because that isn't const.
  *jp = 11;

  // We cannot change what jp points to, though
  int k = 7;
  // jp = &k; <- This is an error

  // This is a pointer that we cannot point somewhere else
  // and where we cannot change what it points to either.
  const int * const kp = &j;

  // It is fine that j isn't const, we can change it
  j = 3;
  // but we cannot change it through kp
  // *kp = 5; <- This is an error

  // We cannot change kp itself either
  // kp = &k; <- This is an error

  return EXIT_SUCCESS;
}
