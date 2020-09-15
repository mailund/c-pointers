#include <stdlib.h>
#include <stdio.h>

#if 0
void pointer(int *a)
{
  printf("pointer: %zu %zu\n", sizeof(a), sizeof(*a));
}

void array(int a[])
{
  printf("array: %zu %zu\n", sizeof(a), sizeof(*a));
}

void array_with_size(int a[50])
{
  printf("array[50]: %zu %zu\n", sizeof(a), sizeof(*a));
}

void array_with_parameter_size(int n, int a[n])
{
  printf("array[n]: %zu %zu\n", sizeof(a), sizeof(*a));
}
#endif

#if 0
void error(int a[n], int n)
{
  // won't compile
}
#endif

void size_constrained(int a[static 4])
{
}

void indirect_size_constrained(int a[static 2])
{
  size_constrained(a);
}

int main(int argc, char **argv)
{
  int n = 100;
  int a[n];

  printf("declared: %zu %zu\n", sizeof(a), sizeof(*a));
  pointer(a);
  array(a);
  array_with_size(a);
  array_with_parameter_size(n, a);

  int b[2];
  size_constrained(b); // Warning
  int *p = b;
  size_constrained(p); // No warning

  indirect_size_constrained(b); // No warning

  return EXIT_SUCCESS;
}
