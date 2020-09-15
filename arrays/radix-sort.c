#include <stdio.h>
#include <stdlib.h>


void bucket_sort(int *input, int *output,
                 int n, int offset)
{
  int buckets[256];
  for (int i = 0; i < 256; i++) {
    buckets[i] = 0;
  }

  for (int i = 0; i < n; i++) {
    unsigned char bucket = (input[i] >> 8 * offset) & 0xff;
    buckets[bucket]++;
  }

  int m = n;
  for (int i = 256 - 1; i >= 0; i--) {
    int bucket = buckets[i];
    buckets[i] = m - bucket;
    m -= bucket;
  }

  for (int i = 0; i < n; i++) {
    unsigned char bucket = (input[i] >> 8 * offset) & 0xff;
    int index = buckets[bucket]++;
    output[index] = input[i];
  }
}

void radix_sort(int *input, int *output, int n)
{
  // helper buffers
  int helper0[n], helper1[n];
  int *helpers[] = { helper0, helper1 };
  for (int i = 0; i < n; i++) {
    helper0[i] = input[i];
  }
  int bucket_input = 0;

  for (int offset = 0; offset < sizeof(int); offset++) {
    bucket_sort(helpers[bucket_input],
                helpers[!bucket_input],
                n, offset);
    bucket_input = !bucket_input;
  }

  for (int i = 0; i < n; i++) {
    output[i] = helpers[!bucket_input][i];
  }
}

int *scan_right(int *left, int *right)
{
  while (left < right) {
    if (*left >= 0) break;
    left++;
  }
  return left;
}

int *scan_left(int *left, int *right)
{
  while (left < right) {
    if (*right < 0) break;
    right--;
  }
  return right;
}

void swap(int *left, int *right)
{
  int i = *left;
  *left = *right;
  *right = i;
}

int *split(int *left, int *right)
{
  while (left < right) {
    left = scan_right(left, right);
    right = scan_left(left, right);
    swap(left, right);
  }
  return left;
}

void reverse(int *left, int *right)
{
  while (left < right) {
    swap(left++, right--);
  }
}


void sort_int(int *input, int *output, int n)
{
  int *left = input;
  int *right = input + n;
  int *x = split(left, right - 1);
  int m = x - left;

  radix_sort(input, output, m);
  reverse(output, output + m - 1);
  radix_sort(input + m, output + m, n - m);
}

int main(int argc, char **argv)
{
  int input[] = { -1, -2, 13, 12, 4, 4200, 13, 6, 14, -3, 42, 13 };
  int n = sizeof(input) / sizeof(int);
  int output[n];

  sort_int(input, output, n);
  for (int i = 0; i < n; i++) {
    printf("%d ", output[i]);
  }
  printf("\n");

  return EXIT_SUCCESS;
}
