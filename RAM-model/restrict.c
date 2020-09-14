
#include <stdio.h>
#include <stdlib.h>

void abc(int *a, int *b, int *c)
{
  *a += *c;
  *b += *c;
}

void abc_restrict(int *a, int *b, int * restrict c)
{
  *a += *c;
  *b += *c;
}

int main(int argc, char **argv)
{
  int a, b, c;
  a = b = 13;
  abc(&a, &b, &a);
  printf("%d %d\n", a, b);

  a = b = 13;
  abc_restrict(&a, &b, &a);
  printf("%d %d\n", a, b);


return EXIT_SUCCESS;
}
