#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

struct array_iter {
  void *array;
  int n; int size;
  int i;
};

void init_array_iter(struct array_iter *iter,
                     void *array, int n, int size)
{
  iter->array = array;
  iter->n = n; iter->size = size;
  iter->i = 0;
}

bool next_element(struct array_iter *iter,
                  void **element)
{
  if (iter->i == iter->n) return false;
  *element = iter->array + iter->i * iter->size;
  iter->i++;
  return true;
}

void cleanup_array_iter(struct array_iter *iter)
{
  // Nothing to do
}

int main(int argc, char **argv)
{
  int array[] = { 1, 2, 3 };
  int n = sizeof(array) / sizeof(*array);

  struct array_iter iter;
  void *element;

  init_array_iter(&iter, array, n, sizeof(int));
  while (next_element(&iter, &element)) {
    printf("%d\n", *(int *)element);
  }
  cleanup_array_iter(&iter);

  init_array_iter(&iter, array, 
                  n * sizeof(int),
                  sizeof(char));
  while (next_element(&iter, &element)) {
    printf("%d\n", *(char *)element);
  }
  cleanup_array_iter(&iter);

  return EXIT_SUCCESS;
}