#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_matrix(int n, int m, int matrix[n][m])
{
  printf("[\n");
  for (int i = 0; i < n; i++) {
    printf("\t[");
    for (int j = 0; j < m; j++) {
      printf("%d ", matrix[i][j]);
    }
    printf("]\n");
  }
  printf("]\n");
}

int main(int argc, char **argv)
{
  int m[2][3] = {
    {1, 2, 3},
    {11, 22, 33}
  };
  int a[2 * 3];
  memcpy(a, m, 2 * 3 * sizeof(int));

  print_matrix(2, 3, m);
  print_matrix(2, 3, (int (*)[3])a);
  print_matrix(2, 2, (int (*)[2])a);
  print_matrix(2, 2, (int (*)[2])m);

  return EXIT_SUCCESS;
}
