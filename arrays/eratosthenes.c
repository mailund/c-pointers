#include <stdlib.h>
#include <stdio.h>


int *eratosthenes(int n, int buf[n])
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
  int *compact = buf;
  for (int *from = buf; from < buf + n; from++) {
    if (*from)
      *compact++ = *from;
  }

  return compact;
}

int *sieve_candidates(int *from, int *to, int p)
{
  int *output = from;
  for ( ; from < to; from++) {
    if (*from % p != 0)
      *output++ = *from;
  }
  return output;
}

int *eratosthenes_(int n, int buf[n])
{
  // Init
  for (int i = 2; i < n; i++) {
    buf[i - 2] = i;
  }

  // Sieve
  int *candidates = buf;
  int *end = buf + n - 2;
  while (candidates < end) {
    int p = *candidates++;
    end = sieve_candidates(candidates, end, p);
  }

  return end;
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

  int *end = eratosthenes(n, buf);
  print_array(buf, end);
  end = eratosthenes_(n, buf);
  print_array(buf, end);

  return EXIT_SUCCESS;
}
