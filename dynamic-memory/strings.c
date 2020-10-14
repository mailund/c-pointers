#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

char const *slice(char const *x, int i, int j)
{
  assert(i <= j);
  size_t len = j - i;

  char *new_buf = malloc(len + 1);
  if (!new_buf) return 0;

  strncpy(new_buf, x + i, len);
  new_buf[len] = '\0';
  return new_buf;
}

char *replace_string(char const *x, int i, int j, char const *y)
{
  size_t xlen = strlen(x);
  size_t ylen = strlen(y);
  size_t len = xlen - (j - i) + ylen;

  char *new_buf = malloc(len + 1);
  if (!new_buf) return 0;

  strncpy(new_buf, x, i);
  strcpy(new_buf + i, y);
  strcpy(new_buf + i + ylen, x + j);

  return new_buf;
}

char *insert_string(char const *x, int i, char const *y)
{
  return replace_string(x, i, i, y);
}

char *delete_string(char const *x, int i, int j)
{
  return replace_string(x, i, j, "");
}

// copies up to, but not including, the zero-terminal
#define string_copy(dst, src) \
{ while (*(src)) *(dst)++ = *(src)++; }

char *join_strings(char const *sep, int n, char const *strings[n])
{
  if (n == 0) {
    // We could consider this an error and return NULL.
    // We could also return an empty string, but we would have to
    // allocate it if we want the interface to this function to be
    // that all strings it returns are dynamically allocated.
    // I will go with the latter.
    char *x = malloc(1);
    if (x) *x = '\0';
    return x;
  }

  size_t no_strings = 0, string_len = 0;
  for (int i = 0; i < n; i++) {
    no_strings++;
    string_len += strlen(strings[i]);
  }
  size_t len = string_len + (no_strings - 1 ) * strlen(sep);
  char *new_buf = malloc(len + 1);
  char *dst = new_buf;
  char const *xsep = "";
  for (int i = 0; i < n; i++) {
    string_copy(dst, xsep);
    string_copy(dst, strings[i]);
    xsep = sep;
  }
  *dst = '\0';

  return new_buf;
}



// using substrings

struct range {
  char *begin;
  char *end;
};

typedef struct range substr;
typedef struct range substr_iter;

// Creating substr
#define SUBSTR(b,e) \
  (substr){ .begin = (b), .end = (e) }

substr as_substr(char const *s)
{
  char *x = (char *)s;
  while (*x) x++;
  return SUBSTR((char *)s, x);
}

// Use a null begin pointer as a special symbol
static substr const NULL_SUBSTR = { .begin = 0 };
#define null_substr(x)  ((x).begin == 0)
#define substr_len(x)   ((x).end - (x).begin)

substr find_occurrence(substr x, substr y)
{
  int n = substr_len(x);
  int m = substr_len(y);
  if (m > n) return NULL_SUBSTR;

  char *s = x.begin, *end = x.end - m;
  for (; s < end; s++) {
    if (strncmp(s, y.begin, m) == 0) {
      return SUBSTR(s, s + m);
    }
  }
  return NULL_SUBSTR;
}

// Iterator for non-overlapping occurrences of s
substr next_occurrence(substr_iter *iter,
                       substr s,
                       int overlaps)
{
  substr occ = find_occurrence(*iter, s);
  if (!null_substr(occ)) {
    // if there was an occurrence, we must update
    // the iterator
    iter->begin = overlaps ? occ.begin + 1 : occ.end;
  }
  return occ;
}

#define range_copy(dst, begin, end)           \
 { for (char const *p = begin; p != end; p++) \
      *(dst)++ = *p;                          \
 }

char *replace_all_occurrences(char const *z, char const *x, char const *y)
{
  substr ssz = as_substr(z);
  substr ssx = as_substr(x);
  substr ssy = as_substr(y);
  size_t zlen = substr_len(ssz);
  size_t xlen = substr_len(ssx);
  size_t ylen = substr_len(ssy);

  size_t no_occurrences = 0;
  substr_iter iter = ssz;
  for (substr occ = next_occurrence(&iter, ssx, 0);
       !null_substr(occ);
       occ = next_occurrence(&iter, ssx, 0)) {
    no_occurrences++;
  }
  size_t len = zlen + no_occurrences * (ylen - xlen);
  char *new_buf = malloc(len + 1);
  if (!new_buf) return 0;

  char const *src = z;
  char *dst = new_buf;
  iter = ssz;
  for (substr occ = next_occurrence(&iter, ssx, 0);
       !null_substr(occ);
       occ = next_occurrence(&iter, ssx, 0)) {
    range_copy(dst, src, occ.begin);
    range_copy(dst, ssy.begin, ssy.end);
    src = occ.end;
  }
  range_copy(dst, src, z + zlen);
  *dst = '\0';

  return new_buf;
}

int main(void)
{
  char const *x = "foobarbaz";
  char *y = (char *)slice(x, 3, 6);
  printf("y is %s\n", y);
  assert(strcmp(y, "bar") == 0);
  free(y);

  y = replace_string(x, 3, 6, "");
  printf("y is %s\n", y);
  assert(strcmp(y, "foobaz") == 0);
  free(y);

  y = replace_string(x, 3, 6, "quxqax");
  printf("y is %s\n", y);
  assert(strcmp(y, "fooquxqaxbaz") == 0);
  free(y);

  y = insert_string(x, 0, "XX");
  printf("y is %s\n", y);
  assert(strcmp(y, "XXfoobarbaz") == 0);
  free(y);

  y = delete_string(x, 0, 3);
  printf("y is %s\n", y);
  assert(strcmp(y, "barbaz") == 0);
  free(y);

  char const *strings[] = {
    "foo", "bar", "baz"
  };
  char *z = join_strings(":", sizeof strings / sizeof *strings, strings);
  printf("z = %s\n", z);
  free(z);

  x = "foobarbazbax";
  z = replace_all_occurrences(x, "ba", "");
  printf("z == %s\n", z);
  free(z);

  z = replace_all_occurrences(x, "a", "OOOO");
  printf("z == %s\n", z);
  free(z);

  z = replace_all_occurrences(x, "123", "OOOO");
  printf("z == %s\n", z);
  free(z);

  return 0;
}
