#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

void not_what_you_want(int array[])
{
  // sizeof(array) is sizeof(int *) here!
  printf("%zu\n", sizeof(array));
  // Here, the array and the address of the array
  // are different. array is a local variable
  // that holds a pointer to the array!
  printf("%p %p\n", array, &array);
}

int main(int argc, char **argv)
{
  int array[] = { 1, 2, 3, 4, 5 };
  int *ap = array;

  printf("sizeof array == %zu, sizeof ap == %zu\n",
         sizeof(array), sizeof(ap));
  printf("%p %p %p %p\n", array, &array, ap, &ap);

  int n = sizeof(array) / sizeof(*array);
  for (int i = 0; i < n; i++) {
    assert(array[i] == ap[i]);
    assert(array + i == ap + i);
    assert(*(array + i) == *(ap + i));
  }

  return EXIT_SUCCESS;
}
