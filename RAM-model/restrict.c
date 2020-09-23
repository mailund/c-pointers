#include <stdio.h>

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

int main(void)
{
  int a, b;
  a = b = 13;
  abc(&a, &b, &a);
  printf("%d %d\n", a, b);

  a = b = 13;
  abc_restrict(&a, &b, &a);
  printf("%d %d\n", a, b);


return 0;
}
