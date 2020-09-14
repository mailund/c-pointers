#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
  int i = 42;
  int *ip = &i;
  void *vp = ip;
  char *cp = vp;


  return EXIT_SUCCESS;
}
