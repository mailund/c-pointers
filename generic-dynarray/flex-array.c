
#include <stdio.h>
#include <stddef.h>


struct S {
  char c[2];
  int array[];
};

struct T {
  char c[2];
  char array[];
};


struct U {
  void *p;
  int i;
  char array[];
};

int main(void)
{
  printf("%zu %zu\n", sizeof(struct S),
         offsetof(struct S, array));
  printf("%zu %zu\n", sizeof(struct T),
         offsetof(struct T, array));
  printf("%zu %zu\n", sizeof(struct U),
         offsetof(struct U, array));

  struct T *t = 0;
  printf("%zu %zu\n", sizeof *t,
         offsetof(__typeof__(*t), array));

  return 0;
}
