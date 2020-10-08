#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

struct range {
  char *begin;
  char *end;
};

typedef struct range substr;

// Use a null begin pointer as a special symbol
static substr const NULL_SUBSTR = { .begin = 0 };
int is_null_substr(substr s)
{
  return s.begin == 0;
}

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

// Creating a string from a substr
char *substr_to_buf(char *to, substr from)
{
  while (from.begin != from.end) {
    *to++ = *from.begin++;
  }
  *to = '\0';
  return to;
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

void substr_rev(substr s)
{
  // We need to point at, not past, the last char
  s.end--;
  while (s.begin < s.end) {
    // Swap
    char x = *s.begin;
    *s.begin = *s.end;
    *s.end = x;
    // increment to next pair
    s.begin++ ; s.end--;
  }
}

int empty_substr(substr x)
{
  return x.begin == x.end;
}

int substr_overlaps(substr x, substr y)
{
  // Empty strings do not overlap
  if (empty_substr(x) || empty_substr(y))
    return 0;

  // Now both strings have begin < end, and they
  // overlap if the left-most ends after the
  // the right-most begins
  if (x.begin < y.begin) {
    return x.end > y.begin;
  } else {
    return y.end > x.begin;
  }
}

// Do they overlap with x on the left?
// This is the only dangerous overlap when it comes to
// copying from x to y
int substr_left_overlaps(substr x, substr y)
{
  return x.begin < y.begin && x.end > y.begin;
}

// We write the charcters in from to to, but only
// part of from if to is shorter than from. The caller
// is responsible for checking substr_left_verlaps(from, to)
// if that can be an issue
substr copy_substr(substr to, substr from)
{
  while (from.begin != from.end &&
         to.begin != to.end) {
    *to.begin++ = *from.begin++;
  }
  // The to substring now goes from the character
  // right after the copy and to the original end
  return to;
}

// It is the caller's responsibility to check that
// x and y have the same length and do not overlap
void swap_substr(substr x, substr y)
{
  while (x.begin != x.end && y.begin != y.end) {
    char c = *x.begin;
    *x.begin = *y.begin;
    *y.begin = c;
    x.begin++; y.begin++;
  }
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

substr next_word(substr *iter)
{
  char *begin = find_word(iter->begin);
  if (*begin == '\0') {
    // no more words
    return NULL_SUBSTR;
  }
  char *end = skip_word(begin);
  iter->begin = end;
  return (substr){
    .begin = begin,
    .end = end
  };
}

// Compacting over iterator...
substr copy_words(substr to, substr from)
{
  // remember where we started
  char *begin = to.begin;
  // sep is used to put spaces between
  // words but not before the first word
  char sep = '\0';
  substr iter = from;
  for (substr word = next_word(&iter);
       !is_null_substr(word);
       word = next_word(&iter)) {
    if (sep) *to.begin++ = sep;
    sep = ' '; // always space after the first
    to = copy_substr(to, word);
  }
  return (substr){
    .begin = begin, .end = to.begin
  };
}

substr compact_words(substr s)
{
  return copy_words(s, s);
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


// Iterator for non-overlapping occurrences of s
substr next_occurrence(substr *iter, substr s)
{
  int n = substr_len(s);
  char *x = iter->begin, *end = iter->end - n;
  for (; x < end; ++x) {
    if (strncmp(x, s.begin, n) == 0) {
      // we found a match
      iter->begin = x + n; // move to after match
      return (substr){
        .begin = x, .end = x + n
      };
    }
  }
  // No more matches...
  return NULL_SUBSTR;
}

// FIXME: handle unequal length
substr replace_substr(substr s, substr from, substr to)
{
  substr iter = s;
  for (substr match = next_occurrence(&iter, from);
       !is_null_substr(match);
       match = next_occurrence(&iter, from)) {
    copy_substr(match, to);
  }
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
  substr_to_buf(buf, ss);
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

  printf("reverse\n");
  do {
    char x[] = "abc";
    substr_rev(as_substr(x));
    printf("%s\n", x);
  } while(0);
  do {
    char x[] = "";
    substr_rev(as_substr(x));
    printf("%s\n", x);
  } while(0);
  do {
    char x[] = "a";
    substr_rev(as_substr(x));
    printf("%s\n", x);
  } while(0);
  do {
    char x[] = "ab";
    substr_rev(as_substr(x));
    printf("%s\n", x);
  } while(0);

  printf("\n");

  // Overlaps
  do {
    // Empty strings do not overlap
    substr x = slice(s, 0, 0);
    substr y = slice(s, 0, 0);
    assert(!substr_overlaps(x, y));
  } while(0);
  do {
    // Empty strings do not overlap
    substr x = slice(s, 0, 1);
    substr y = slice(s, 0, 0);
    assert(!substr_overlaps(x, y));
    assert(!substr_overlaps(y, x));
  } while(0);

  do {
    substr x = slice(s, 0, 1);
    substr y = slice(s, 0, 1);
    assert(substr_overlaps(x, y));
    assert(substr_overlaps(y, x));
  } while(0);
  do {
    substr x = slice(s, 0, 5);
    substr y = slice(s, 0, 5);
    assert(substr_overlaps(x, y));
    assert(substr_overlaps(y, x));
  } while(0);
  do {
    substr x = slice(s, 0, 5);
    substr y = slice(s, 5, 10);
    assert(!substr_overlaps(x, y));
    assert(!substr_overlaps(y, x));
  } while(0);

  do {
    substr x = slice(s, 1, 5);
    substr y = slice(s, 3, 10);
    assert(substr_overlaps(x, y));
    assert(substr_overlaps(y, x));
  } while(0);

  do {
    substr x = slice(s, 1, 5);
    substr y = slice(s, 3, 10);
    assert(substr_left_overlaps(x, y));
    assert(!substr_left_overlaps(y, x));
  } while(0);


  // Copying
  printf("copying\n");
  do {
    char w[] = "foobar";
    substr x = slice(w, 0, 3);
    substr y = slice(w, 0, 3);
    substr z = copy_substr(x, y);
    assert(substr_len(z) == 0); // we copied all, so it should be empty
    printf("%s\n", z.end); // the rest of w, so bar
  } while(0);
  do {
    char w[] = "foobar";
    // This violates the left overlap, but the result is correct,
    // it is fffbar (we are copying the first f to pos 1 and 2)
    substr x = slice(w, 1, 3);
    substr y = slice(w, 0, 3);
    substr z = copy_substr(x, y);
    assert(substr_len(z) == 0); // we copied all, so it should be empty
    printf("%s\n", w);
    printf("%s\n", z.end); // the rest of w, so bar
  } while(0);
  do {
    char w[] = "foobar";
    substr x = slice(w, 0, 6);
    substr y = slice(w, 3, 6);
    substr z = copy_substr(x, y);
    assert(substr_len(z) == 3); // bar is still there
    printf("%s\n", w); // barbar
    print_substr(z); printf("\n"); // bar
  } while(0);
  do {
    char w[] = "foobar";
    substr x = slice(w, 0, 2);
    substr y = slice(w, 3, 6);
    substr z = copy_substr(x, y);
    assert(substr_len(z) == 0); // we filled the input
    printf("%s\n", w); // baobar
  } while(0);
  printf("\n");

  printf("swapping\n");
  do {
    char w[] = "foobar";
    substr x = slice(w, 0, 3);
    substr y = slice(w, 3, 6);
    swap_substr(x, y);
    printf("%s\n", w); // barfoo
  } while(0);
  printf("\n");

  printf("iterating over words\n");
  char x[] = "\tfoo    bar123baz\nqux321";
  substr iter = as_substr(x);
  for (substr word = next_word(&iter);
       !is_null_substr(word);
       word = next_word(&iter)) {

    print_substr(word); printf("\n");

    char c = zero_term(word);
    printf("%s\n", word.begin);
    restore_term(word, c);
  }
  printf("\n");

  substr xx = compact_words(as_substr(x));
  *xx.end = '\0';
  printf("compacted words: \"%s\"\n", x);

  printf("finding patterns\n");
  iter = as_substr(x);
  substr ba = as_substr("ba");
  for (substr word = next_occurrence(&iter, ba);
       !is_null_substr(word);
       word = next_occurrence(&iter, ba)) {

    print_substr(word); printf("\n");
  }
  printf("\n");

  replace_substr(as_substr(x), ba, as_substr("fo"));
  printf("%s\n", x);


  return 0;
}
