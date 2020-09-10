#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void sort_chars(const char *input, char *output)
{
  int buckets[256];
  for (int i = 0; i < 256; i++) {
    buckets[i] = 0;
  }

  while (*input) {
    unsigned char bucket = *input;
    buckets[bucket]++;
    input++;
  }

  int k = 0;
  for (int i = 0; i < 256; i++) {
    for (int j = 0; j < buckets[i]; j++) {
      output[k++] = (char)i;
    }
  }
}

int main(int argc, char **argv)
{
  const char *input = "foobar";
  int n = strlen(input);
  char output[n + 1]; output[n] = '\0';

  sort_chars(input, output);
  printf("%s -> %s\n", input, output);

  return EXIT_SUCCESS;
}
