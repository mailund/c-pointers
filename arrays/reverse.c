#include <stdio.h>
#include <string.h>

void reverse(void *array, int n, int size)
{
  char *left = array;
  char *right = array + size * (n - 1);
  char tmp[size];

  while (left < right) {
    memcpy(&tmp, left, size);
    memcpy(left, right, size);
    memcpy(right, &tmp, size);
    left += size; right -= size;
  }
}

int main(void)
{
  int int_array[] = { 1, 2, 3, 4, 5 };
  int n = sizeof(int_array) / sizeof(int);
  reverse(int_array, n, sizeof(int));
  for (int i = 0; i < n; i++) {
    printf("%d ", int_array[i]);
  }
  printf("\n");

  char char_array[] = { 'f', 'o', 'o', 'b', 'a', 'r' };
  int m = sizeof(char_array) / sizeof(char);
  reverse(char_array, m, sizeof(char));
  for (int i = 0; i < m; i++) {
    printf("%c ", char_array[i]);
  }
  printf("\n");

  return 0;
}
