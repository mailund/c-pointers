#include <stdlib.h>
#include <stdio.h>

int compact0(int n, int array[n])
{
  int m = 0;
  for (int i = 0; i < n; i++) {
    if (array[i] > 0)
      array[m++] = array[i];
  }
  return m;
}

int eratosthenes(int n, int buf[n])
{
  // Init
  buf[0] = buf[1] = 0;
  for (int i = 2; i < n; i++) {
    buf[i] = i;
  }

  // Sieve
  for (int i = 2; i*i < n; i++) {
    if (buf[i] == 0) continue;
    int p = buf[i];
    for (int j = p*p; j < n; j += p) {
      buf[j] = 0;
    }
  }

  // Compact
  return compact0(n, buf);
}

void sieve_candidates(int **from, int **to)
{
  int p = *(*from)++; // get the prime and move from
  int *output = *from;
  for (int *input = *from ; input < *to; input++) {
    if (*input % p != 0)
      *output++ = *input;
  }
  *to = output;
}

int eratosthenes_(int n, int buf[n])
{
  // Init
  for (int i = 2; i < n; i++) {
    buf[i - 2] = i;
  }

  // Sieve
  int *candidates = buf;
  int *end = buf + n - 2;
  while (candidates < end) {
    sieve_candidates(&candidates, &end);
  }

  return end - buf;
}


void print_array(int *begin, int *end)
{
  while (begin < end) {
    printf("%d ", *begin++);
  }
  printf("\n");
}

int main(int argc, char **argv)
{
  int n = 100;
  int buf[n];

  int m = eratosthenes(n, buf);
  print_array(buf, buf + m);
  m = eratosthenes_(n, buf);
  print_array(buf, buf + m);

  return EXIT_SUCCESS;
}
