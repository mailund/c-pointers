#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int strlen_index(const char *s)
{
  int i = 0;
  while (s[i])
    i++;
  return i;
}

int strlen_pointer(const char *s)
{
  const char *x = s;
  while (*x)
    x++;
  return x - s;
}

void string_copy(char *output, const char *input)
{
  const char *x = input;
  char *y = output;
  for ( ; *x ; x++, y++ )
    *y = *x;
  *y = '\0';
}

void string_copy_(char *output, const char *input)
{
  while (*input) {
    *output++ = *input++;
  }
  *output = '\0';
}

void string_copy__(char *output, const char *input)
{
  while ( (*output = *input) ) {
    output++; input++;
  }
}

void string_copy___(char *output, const char *input)
{
  while ( (*output++ = *input++) ) ;
}



int main(int argc, char **argv)
{
  const char *string = "hello, world\n";
  int n = strlen(string);
  char buffer[n + 1];
  string_copy___(buffer, string);
  printf("%s", buffer);

  return EXIT_SUCCESS;
}
