#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

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
  *output = '\0';
}

int main(int argc, char **argv)
{
  const char *x = "aabaabbbcbcc";
  char buffer[2 * strlen(x) + 1];

  runlength_encode(x, buffer);
  printf("%s\n", buffer);

  return EXIT_SUCCESS;
}
