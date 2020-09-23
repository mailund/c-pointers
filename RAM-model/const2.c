#include <stdio.h>

void f1(void)
{
  printf("---------- f1() -----------------\n");

  int i = 1, j = 2, k = 3;
  int *ip = &i, *jp = &j;

  int **ipp = &ip;
  // We can change ipp, it isn't const
  ipp = &jp;
  // We can change what ipp points to, that isn't constant
  *ipp = &k;
  // Since *ipp was jp, we just changed jp to it points to k.
  printf("%p == %p\n", jp, &k);
  // We can change through the pointers, the int isn't const
  **ipp = 42; // This changes k
  printf("k is now %d\n", k);

  printf("---------------------------------\n\n");
}

void f2(void)
{
  printf("---------- f2() -----------------\n");

  int i = 1, j = 2;
  int *ip = &i, *jp = &j;

  int ** const ipp = &ip;
  // We cannot change ipp
  // ipp = &jp; <- This is an error
  // We can change what ipp points to, that isn't constant
  *ipp = &j;
  // Since *ipp was &ip, we changed ip to point to j.
  printf("%p == %p\n", ip, &j);

  // We can change through the pointers, the int isn't const
  **ipp = 13; // This changes j
  printf("j is now %d\n", j);

  printf("---------------------------------\n\n");
}

void f3(void)
{
  printf("---------- f3() -----------------\n");

  int i = 1, j = 2;
  int *ip = &i, *jp = &j;

  int * const * ipp = &ip;
  // We can change ipp
  ipp = &jp;
  // We cannot change what it points to, that is const
  // *ipp = &j; <- this is an error

  // We can change through the pointers, the int isn't const
  **ipp = 42; // This changes j
  printf("j is now %d\n", j);

  printf("---------------------------------\n\n");
}

void x(void)
{
  const int i = 13;
  int *ip;
  const int **ipp = (const int **)&ip;
  *ipp = &i; // const int * = const int * -- OK
  printf("&i == %p, *ip == %p\n", &i, ip);
  *ip = 5; // DANGER: We are trying to change const int i to 5
  // This may or may not actually change i. It is up to the C compiler
}

void foo(const int *cip)
{
  int *ip = (int *)cip;
  *ip = 5;
}

void f4(void)
{
  printf("---------- f4() -----------------\n");

  int i = 1, j = 2;
  int *ip = &i, *jp = &j;

  int const ** ipp = (const int **)&ip;
  // We can change ipp
  ipp = (const int **)&jp;
  // We can change what it points to
  *ipp = &j;

  // We cannot change through the pointers, the int is const
  // **ipp = 42; <- This is an error

  printf("---------------------------------\n\n");
}

void f5(void)
{
  printf("---------- f5() -----------------\n");

  int i = 1, j = 2;
  int *ip = &i, *jp = &j;

  int * const * const ipp = &ip;
  // We can't change ipp
  // ipp = (const int **)&jp; <- This is an error
  // We can't change what it points to
  // *ipp = &j; <- This is an error

  // We can change through the pointers, the int isn't const
  **ipp = 42;
  printf("i is now %d\n", i);

  printf("---------------------------------\n\n");
}

int main(void)
{
  f1();
  f2();
  f3();
  x();
  f4();
  f5();

  const int k = 42;
  int *ip = (int *)&k;
  *ip = 13; // i = 13
  printf("i == %d, *ip == %d\n", k, *ip);

  const int i = 42;
  int j = 13;
  foo(&i);
  foo(&j);
  printf("i == %d, j == %d\n", i, j);

  return 0;
}
