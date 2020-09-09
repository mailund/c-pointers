#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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

int main(int argc, char **argv)
{
  int int_array[] = { 10, 5, 30, 15, 20, 30 };
  int int_array_length = sizeof(int_array) / sizeof(int);

  qsort(int_array, int_array_length, sizeof(int), int_compare);
  printf("int_array = ");
  for (int i = 0; i < int_array_length; i++) {
    printf("%d, ", int_array[i]);
  }
  printf("\n");

  char *string_array[] = { "foo", "bar", "baz" };
  int string_array_length = sizeof(string_array) / sizeof(char *);

  qsort(string_array, string_array_length, sizeof(char *), string_compare);
  printf("string_array = ");
  for (int i = 0; i < string_array_length; i++) {
    printf("%s, ", string_array[i]);
  }
  printf("\n");

  return EXIT_SUCCESS;
}
