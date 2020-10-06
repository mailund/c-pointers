#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <assert.h>

struct range {
  char *begin;
  char *end;
};

typedef struct range substr;

// Creating substr
substr as_substr(char *s)
{
  char *x = s;
  while (*x) x++;
  return (substr){
    .begin = s,
    .end = x
  };
}

substr slice(char *s, int i, int j)
{
  assert(i <= j);
  return (substr){
    .begin = s + i,
    .end   = s + j
  };
}

// IO
void print_substr(substr s)
{
  while (s.begin != s.end) {
    putchar(*s.begin++);
  }
}

// Basic operations
size_t substr_len(substr s)
{
  return s.end - s.begin;
}

int substr_cmp(substr x, substr y)
{
  while (x.begin != x.end && y.begin != y.end) {
    if (*x.begin < *y.begin) return -1;
    if (*x.begin > *y.begin) return +1;
    x.begin++; y.begin++;
  }
  // We've reached the end of one of the substrings.
  // If they had the same length, they are equal,
  // otherwise, the shorter string is the smallest
  if (x.begin < x.end) return +1; // x is longer
  if (y.begin < y.end) return -1; // y is longer
  return 0; // the strings are equal
}

char *substr_copy(char *to, substr from)
{
  while (from.begin != from.end) {
    *to++ = *from.begin++;
  }
  *to = '\0';
  return to;
}

// Word iteration
char *skip_word(char *x)
{
  while (*x && isalpha(*x))
    x++;
  return x;
}

char *find_word(char *x)
{
  while (*x && !isalpha(*x))
    x++;
  return x;
}

substr first_word(char *s)
{
  char *begin = find_word(s);
  if (*begin == '\0') {
    // no more words
    return (substr){ .begin = 0 };
  }
  char *end = skip_word(begin);
  return (substr){
    .begin = begin,
    .end = end
  };
}

substr next_word(substr s)
{
  return first_word(s.end);
}

bool done_iterating(substr s)
{
  if (s.begin == 0) return true;
  else return false;
}

// Compacting over iterator...
void copy_words(char *from, char *to)
{
  // sep is used to put spaces between
  // words but not before the first word
  char sep = '\0';
  for (substr word = first_word(from);
       !done_iterating(word);
       word = next_word(word)) {
    if (sep) *to++ = sep;
    sep = ' '; // always space after the first
    to = substr_copy(to, word);
  }
}

void compact_words(char *s)
{
  copy_words(s, s);
}

// Mutating substrings to create strings
char zero_term(substr s)
{
  char c = *s.end;
  *s.end = '\0';
  return c;
}

void restore_term(substr s, char c)
{
  *s.end = c;
}

int main(void)
{
  char s[] = "hello, world";
  substr ss = as_substr(s);

  printf("%zu %zu\n", strlen(s), substr_len(ss));

  printf("slicing \""); print_substr(ss); printf("\"");
  ss = slice(s, 0, 5);
  printf(" gives us \""); print_substr(ss); printf("\"\n");
  printf("%zu %zu\n", strlen(s), substr_len(ss));

  char buf[substr_len(ss) + 1];
  substr_copy(buf, ss);
  printf("%s\n", buf);

  char c = zero_term(ss);
  printf("%s\n", ss.begin);
  restore_term(ss, c);

  printf("comparisons\n");
  printf("aa vs a: %d\n", substr_cmp(as_substr("aa"), as_substr("a")));
  printf("a vs aa: %d\n", substr_cmp(as_substr("a"), as_substr("aa")));
  printf("aa vs ab: %d\n", substr_cmp(as_substr("aa"), as_substr("ab")));
  printf("aa vs aa: %d\n", substr_cmp(as_substr("aa"), as_substr("aa")));
  printf("\n");

  printf("iterating over words\n");
  char x[] = "\tfoo    bar123baz\nqux321";
  for (substr word = first_word(x);
       !done_iterating(word);
       word = next_word(word)) {

    print_substr(word); printf("\n");

    char c = zero_term(word);
    printf("%s\n", word.begin);
    restore_term(word, c);
  }
  printf("\n");

  compact_words(x);
  printf("compacted words: \"%s\"\n", x);

  return 0;
}
