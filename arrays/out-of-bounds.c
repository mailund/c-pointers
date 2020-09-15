#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
  int n = 5;
  int buffer[n];
  int *a = buffer - 1; // 1-index buffer

  for (int i = 1; i <= n; i++) {
    a[i] = i;
  }
  for (int i = 1; i <= n; i++) {
    printf("%d ", a[i]);
  }
  printf("\n");

  int *end = buffer + n;
  for (int *p = buffer; p < end; p++) {
    printf("%d ", *p);
  }
  printf("\n");

  return EXIT_SUCCESS;
}