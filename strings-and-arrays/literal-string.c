#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char **argv)
{
  char *string = "foo";
  string[1] = 'O'; // Changing foo to fOo?
  printf("Succes! string is now '%s'\n", string);
  return EXIT_SUCCESS;
}
