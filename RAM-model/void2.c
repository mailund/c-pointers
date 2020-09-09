#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

int int_compare(const void *x, const void *y)
{
  // Get the objects, and interpret them as integers
  int *a = (int *)x;
  int *b = (int *)y;
  return *a - *b;
}

int string_compare(const void *x, const void *y)
{
  // Get the objects and interpet them as strings
  char **a = (char **)x;
  char **b = (char **)y;
  return strcmp(*a, *b);
}

typedef int (*compare_function)(const void *, const void *);
bool is_sorted(const void *array,
               size_t len, size_t obj_size,
               compare_function cmp)
{
  for (int i = 1; i < len; i++) {
    const void *a = array + (i - 1) * obj_size;
    const void *b = array + i * obj_size;
    if (cmp(a, b) > 0) {
      // a is larger than b, so the array is not sorted
      return false;
    }
  }
  return true;
}

int main(int argc, char **argv)
{
  int int_array[] = { 10, 5, 30, 15, 20, 30 };
  int int_array_length = sizeof(int_array) / sizeof(int);

  if (is_sorted(int_array, int_array_length,
                sizeof(int), int_compare)) {
    printf("int_array is sorted\n");
  } else {
    printf("int_array is not sorted\n");
  }
  qsort(int_array, int_array_length, sizeof(int), int_compare);
  if (is_sorted(int_array, int_array_length,
                sizeof(int), int_compare)) {
    printf("int_array is sorted\n");
  } else {
    printf("int_array is not sorted\n");
  }

  char *string_array[] = { "foo", "bar", "baz" };
  int string_array_length = sizeof(string_array) / sizeof(char *);

  if (is_sorted(string_array, string_array_length,
                sizeof(char *), string_compare)) {
    printf("string_array is sorted\n");
  } else {
    printf("string_array is not sorted\n");
  }
  qsort(string_array, string_array_length, sizeof(char *), string_compare);
  if (is_sorted(string_array, string_array_length,
                sizeof(char *), string_compare)) {
    printf("string_array is sorted\n");
  } else {
    printf("string_array is not sorted\n");
  }

  return EXIT_SUCCESS;
}
