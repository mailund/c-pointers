#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{

  int a[] = { 1, 2, 3, 4, 5 };
  int n = sizeof(a) / sizeof(int);

  for (int i = 0; i < n; i++) {
    printf("%d = [", a[i]);
    char *cp = (char *)(a + i);
    for (int j = 0; j < sizeof(int); j++) {
      printf(" %d ", cp[j]);
    }
    printf("]\n");
  }

  return EXIT_SUCCESS;
}
