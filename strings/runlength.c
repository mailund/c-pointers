#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

const char *skip(const char *x)
{
  char c = *x; assert(c != '\0');
  while (*x == c) x++;
  return x;
}

void runlength_encode(const char *input, char *output)
{
  while (*input) {
    char c = *input;
    const char *next = skip(input);
    int length = next - input;
    output += sprintf(output, "%d%c", length, c);
    input = next;
  }
}

int main(int argc, char **argv)
{
  const char *x = "aaaabbbbbbbaabbbcbbccccc";
  char buffer[2 * strlen(x) + 1];

  runlength_encode(x, buffer);
  printf("%s\n", buffer);

  return EXIT_SUCCESS;
}
