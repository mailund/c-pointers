#include <stdio.h>

void safe(int *ip)
{
  printf("*ip == %d\n", *ip);
}

int *unsafe(void)
{
  int i = 42;
  return &i;
}

int main(void)
{
  int *ip = unsafe();
  printf("*ip == %d\n", *ip);

  int i = 42;
  safe(&i);

  printf("*ip == %d\n", *ip);

  return 0;
}
