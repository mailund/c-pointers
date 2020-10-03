#include <stdio.h>

int main(void)
{
  int *i_p;
  int **i_p_p = &i_p;
  int const ** ic_p_p = (int const **)i_p_p;

  int const ci = 42;
  // I can assign to *ic_p_p because its type is
  // int const *, and that is the type if &ci
  *ic_p_p = &ci;

  // but now I have an int alias to an int const!
  printf("&ci == %p, *i_p == %p\n", (void *)&ci, (void *)i_p);

  *i_p = 5; // DANGER: We are trying to change const int i to 5
  // This may or may not actually change i. It is up to the C compiler
  printf("ci == %d / %d\n", ci, *i_p);

  return 0;
}
