#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
  void *vp = 0;
  char *cp = 0;
  int *ip = 0;
  long *lp = 0;

  for (int i = 0; i < 5; i++) {
    printf("char: %p %p ",  cp + i, vp + i * sizeof(char));
    printf("int: %p %p ",   ip + i, vp + i * sizeof(int));
    printf("long: %p %p\n", lp + i, vp + i * sizeof(long));
  }

  return EXIT_SUCCESS;
}
