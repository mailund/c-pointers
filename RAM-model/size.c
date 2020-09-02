#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv)
{
  char c;
  printf("%zu %zu\n", sizeof(char), sizeof(c));
  int i;
  printf("%zu %zu\n", sizeof(int), sizeof(i));
  double d;
  printf("%zu %zu\n", sizeof(double), sizeof(d));
  return EXIT_SUCCESS;
}
