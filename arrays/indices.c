#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

int main(int argc, char **argv)
{
  int array[5] = { 0, 1, 2, 3, 4 };
  int *ip = array;

  for (int i = 0; i < 5; i++) {
    assert(array + i == ip + i);
    assert(array[i] == ip[i]);
    assert(array[i] == *(array + i));
    assert(ip[i] == *(ip + i));

    assert(i[array] == i[ip]);
  }

  void *p = array;
  for (int i = 0; i < 5; i++) {
    printf("%p %p %p\n",
           // int array has the right offset
           array + i,
           // int * has the right offset
           ip + i,
           // void * jumps in bytes...
           p + i * sizeof(int));
  }

  int *end = array + sizeof(array) / sizeof(int);
  for (ip = array; ip < end; ip++) {
    printf("%p\n", ip);
  }

  void *vend = (void *)end;
  for (p = array; p < vend; p += sizeof(int)) {
    printf("%p\n", p);
  }

  return EXIT_SUCCESS;
}
