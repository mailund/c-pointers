#ifndef STRINGS_H
#define STRINGS_H

#include <stdlib.h>

char const *slice(char const *x, int i, int j);
char *replace_string(char const *x, int i, int j, char const *y);
char *insert_string(char const *x, int i, char const *y);
char *delete_string(char const *x, int i, int j);
char *join_strings(char const *sep, int n, char const *strings[n]);

#endif
