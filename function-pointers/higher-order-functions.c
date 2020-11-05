#include "list.h"
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
  int a[] = { 1, 2, 3, 4, 10, 11, 12, 13, 14 };
  int n = sizeof a / sizeof *a;

  list x = make_list(n, a);
  print_list(x);
  free_list(x);
  return 0;
}
