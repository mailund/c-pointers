#include <stdlib.h>

int main(int argc, char **argv)
{
  int array[5];

  int *a1 = array;
  int *a2 = (int *)((void *)array + 1);

  int x = 0, y = 0;
  for (int i = 0; i < 4; i++) {
    x += a1[i];
    y += a2[i];
  }

  return EXIT_SUCCESS;
}
