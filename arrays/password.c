#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

bool validate_password(const char *password)
{
  char buffer[10];
  bool valid_password = false;

  printf("Password: ");
  gets(buffer);

  if (strcmp(password, buffer) == 0)
    valid_password = true;

  return valid_password;
}

int main(int argc, char **argv)
{
  const char *password = "foobar";
  bool valid = validate_password(password);
  if (valid) {
    printf("The password is valid\n");
    return EXIT_FAILURE;
  } else {
    printf("The password is invalid\n");
    return EXIT_SUCCESS;
  }
}
