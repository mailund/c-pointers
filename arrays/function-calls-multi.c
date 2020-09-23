#include <stdio.h>

void array_full_size(int A[10][10], const char *name)
{
  printf("full: sizeof(%s) == %zu (%zu), ",
         name, sizeof(A), sizeof(int (*)[10]));
  printf("sizeof(*%s) == %zu (%zu)\n",
         name, sizeof(*A), 10 * sizeof(int));
}

void array_incomplete_size(int A[][10], const char *name)
{
  printf("incomplete: sizeof(%s) == %zu (%zu), ",
         name, sizeof(A), sizeof(int (*)[10]));
  printf("sizeof(*%s) == %zu (%zu)\n",
         name, sizeof(*A), 10 * sizeof(int));
}

void pointer(int (*A)[10], const char *name)
{
  printf("pointer: sizeof(%s) == %zu (%zu), ",
         name, sizeof(A), sizeof(int (*)[10]));
  printf("sizeof(*%s) == %zu (%zu)\n",
         name, sizeof(*A), 10 * sizeof(int));
}

int main(void)
{
  int A[10][10];
  printf("main: sizeof(A) == %zu\n", sizeof(A));
  array_full_size(A, "A");
  array_incomplete_size(A, "A");
  pointer(A, "A");
  printf("\n");

  int B[5][10];
  printf("main: sizeof(B) == %zu\n", sizeof(B));
  // B's first dimension is wrong, but no warnings
  array_full_size(B, "B");
  array_incomplete_size(B, "B");
  pointer(B, "B");
  printf("\n");

  int C[10][5];
  printf("main: sizeof(C) == %zu\n", sizeof(C));
  // You get warnings here, because the
  // second dimension doesn't match
  array_full_size(C, "C");
  array_incomplete_size(C, "C");
  pointer(C, "C");
  printf("\n");

  return 0;
}
