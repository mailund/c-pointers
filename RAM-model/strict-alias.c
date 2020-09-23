#include <stdio.h>

int f(int *i, long *l)
{
  *i = -1;
  *l = 0;
  return *i;
}

int g(char *c, long *l)
{
  *c = -1;
  *l = 0;
  return *c;
}

int main(void)
{
  long x;
  int i = f((int *)&x, &x);
  int j = g((char *)&x, &x);
  printf("%ld %d %d\n", x, i, j);

  return 0;
}
