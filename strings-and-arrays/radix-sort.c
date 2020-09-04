#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


void bucket_sort(int *input, int *output, int n, int offset)
{
  int buckets[256];
  for (int i = 0; i < 256; i++) {
    buckets[i] = 0;
  }

  for (int i = 0; i < n; i++) {
    unsigned char *integer = (unsigned char *)(input + i);
    unsigned char bucket = integer[offset];
    buckets[bucket]++;
  }

  int m = n;
  for (int i = 256 - 1; i >= 0; i--) {
    int bucket = buckets[i];
    buckets[i] = m - bucket;
    m -= bucket;
  }

  for (int i = 0; i < n; i++) {
    unsigned char *integer = (unsigned char *)(input + i);
    unsigned char bucket = integer[offset];
    int index = buckets[bucket]++;
    output[index] = input[i];
  }
}

void sort_int(int *input, int *output, int n)
{
  // helper buffers
  int helper0[n], helper1[n];
  int *helpers[] = { helper0, helper1 };
  for (int i = 0; i < n; i++) {
    helper0[i] = input[i];
  }
  bool bucket_input = false;

  for (int offset = 0; offset < sizeof(int); offset++) {
    bucket_sort(helpers[bucket_input], helpers[!bucket_input], n, offset);
    bucket_input = !bucket_input;
  }

  for (int i = 0; i < n; i++) {
    output[i] = helpers[!bucket_input][i];
  }
}

int main(int argc, char **argv)
{
  int input[] = { 'a', 'b', 'c', 12, 4, 42000, 13, 6, 14, 870, -1, -2, -3 };
  int n = sizeof(input) / sizeof(int);
  int output[n];

  sort_int(input, output, n);

  printf("output: ");
  for (int i = 0; i < n; i++) {
    printf("%d ", output[i]);
  }
  printf("\n");

  return EXIT_SUCCESS;
}
