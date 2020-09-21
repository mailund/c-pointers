#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv)
{
  int A[2][3] = {
    { 1, 2, 3 },
    { 4, 5, 6 }
  };

  printf("%zu %zu\n", sizeof(A), 2 * 3 * sizeof(int));
  printf("%zu %zu\n", sizeof(*A), 3 * sizeof(int));
  printf("%zu %zu\n", sizeof(A[0]), 3 * sizeof(int));
  printf("%zu %zu\n", sizeof(A[0][0]), sizeof(int));

  int *p = (int *)A;
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 3; j++) {
      printf("A[%d][%d] == %d sits at address %p == %p\n",
             i, j, A[i][j], &A[i][j], p++);
    }
  }

  int B[2][2][3] = {
    { { 1, 2, 3 }, {  4,  5,  6 } },
    { { 7, 8, 9 }, { 10, 11, 12 } }
  };

  printf("%zu %zu\n", sizeof(B), 2 * 2 * 3 * sizeof(int));
  printf("%zu %zu\n", sizeof(B[0]), 2 * 3 * sizeof(int));
  printf("%zu %zu\n", sizeof(B[0][0]), 3 * sizeof(int));
  printf("%zu %zu\n", sizeof(B[0][0][0]), sizeof(int));

  p = (int *)B;
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      for (int k = 0; k < 3; k++) {
        printf("B[%d][%d][%d] == %d sits at address %p == %p\n",
                i, j, k, B[i][j][k], &B[i][j][k], p++);
      }
    }
  }

  return EXIT_SUCCESS;
}
