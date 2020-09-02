#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv)
{
  char c = 1;
  printf("%d %p\n", c, &c);
  int i = 2;
  printf("%d %p\n", i, &i);
  double d = 3.0;
  printf("%f %p\n", d, &d);
  return EXIT_SUCCESS;
}
