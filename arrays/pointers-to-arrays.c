#include <stdio.h>

int main(void)
{
  int array[10];
  int (*ap1)[] = &array;
  int (*ap2)[10] = &array;
  int (*ap3)[5] = &array; // Warning
  int (*ap4)[20] = &array; // Warning
  int *ip = array;

  printf("%p, sizeof array == %zu\n", array, sizeof(array));
  // We cannot get sizeof *ap1, it is an incomplete type.
  printf("%p\n", *ap1);
  printf("%p, sizeof *ap2 == %zu (%zu)\n",
         *ap2, sizeof(*ap2), 10 * sizeof(int));
  printf("%p, sizeof *ap3 == %zu (%zu)\n",
         *ap3, sizeof(*ap3), 5 * sizeof(int));
  printf("%p, sizeof *ap4 == %zu (%zu)\n",
         *ap4, sizeof(*ap4), 20 * sizeof(int));
  printf("%p, sizeof *ip == %zu (%zu)\n",
         ip, sizeof(ip), sizeof(int *));

  return 0;
}
