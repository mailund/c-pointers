#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

struct range {
  char *begin;
  char *end;
};

typedef struct range substr;
typedef substr substr_iter;

// Use a null begin pointer as a special symbol
static substr const NULL_SUBSTR = { .begin = 0 };
int is_null_substr(substr s)
{
  return s.begin == 0;
}

// Creating substr
#define SUBSTR(b,e) \
  (substr){ .begin = (b), .end = (e) }

substr as_substr(char *s)
{
  char *x = s;
  while (*x) x++;
  return SUBSTR(s, x);
}

substr slice(char *s, int i, int j)
{
  assert(i <= j);
  return SUBSTR(s + i, s + j);
}

// Creating a string from a substr.
// The caller is responsible for ensuring that
// the to buffer is large enough (i.e. at least
// substr_len(from) + 1 long).
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
  char * restrict x = s.begin;
  char * restrict y = s.end - 1;
  for (; x < y; x++, y--) {
    char c = *x; *x = *y; *y = c;
  }
}

// The tests for overlaps and such is only valid,
// according to the langauge standard, if the
// substrings are ranges of the same buffer. Comparing
// pointers into separately allocated memory gives no
// guarantees. But it is the best we can do here...

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
  char * restrict x = to.begin;
  char * restrict y = from.begin;
  while (x != to.end && y != from.end)
    *x++ = *y++;

  // The to substring now goes from the character
  // right after the copy and to the original end
  return SUBSTR(x, to.end);
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

// It is the caller's responsibility to check that
// x and y have the same length and do not overlap
void swap_substr(substr x, substr y)
{
  char * restrict p = x.begin;
  char * restrict q = y.begin;
  for (; p != x.end && q != y.end; p++, q++) {
    char c = *p; *p = *q; *q = c;
  }
}

// Is x a sub-range of y?
int is_subrange(substr x, substr y)
{
  return y.begin <= x.begin && x.end <= y.end;
}

// Remove substr y from substr x, put the result
// in to and return the updated substr.
// It is the caller's responsibility to check that
// y is a subrange of x.
substr delete_substr(substr to, substr x, substr y)
{
  substr before = SUBSTR(x.begin, y.begin);
  substr after = SUBSTR(y.end, x.end);
  substr tmp = copy_substr(to, before);
  tmp = copy_substr(tmp, after);
  return SUBSTR(to.begin, tmp.begin);
}

// With this one, remember that you are modifying
// x, so other references to it, and subtrings in it
// will also be affected
substr delete_substr_inplace(substr x, substr y)
{
  substr dest = SUBSTR(y.begin, x.begin);
  substr after = SUBSTR(y.end, x.end);
  substr replacement = copy_substr(dest, after);
  return SUBSTR(x.begin, replacement.begin);
}

// Find the first occurrence of the string y
// in the string x. Return a NULL string if
// there isn't one.
substr find_occurrence(substr x, substr y)
{
  int n = substr_len(y);
  char *s = x.begin, *end = x.end - n;
  for (; s < end; s++) {
    if (strncmp(s, y.begin, n) == 0) {
      return SUBSTR(s, s + n);
    }
  }
  return NULL_SUBSTR;
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

substr next_word(substr_iter *iter)
{
  char *begin = find_word(iter->begin);
  if (*begin == '\0') {
    // no more words
    return NULL_SUBSTR;
  }
  char *end = skip_word(begin);
  iter->begin = end;
  return SUBSTR(begin, end);
}

// Compacting over iterator...
substr copy_words(substr to, substr from)
{
  // remember where we started
  char *begin = to.begin;
  // sep is used to put spaces between
  // words but not before the first word
  char sep = '\0';
  substr_iter iter = from;
  for (substr word = next_word(&iter);
       !is_null_substr(word);
       word = next_word(&iter)) {
    if (sep) *to.begin++ = sep;
    sep = ' '; // always space after the first
    to = copy_substr(to, word);
  }
  return SUBSTR(begin, to.begin);
}

substr compact_words(substr s)
{
  return copy_words(s, s);
}



// Iterator for non-overlapping occurrences of s
substr next_occurrence(substr_iter *iter, substr s)
{
  substr occ = find_occurrence(*iter, s);
  if (!is_null_substr(occ)) {
    // if there was an occurrence, we must update
    // the iterator
    iter->begin = occ.end;
  }
  return occ;
}

// FIXME: handle unequal length
substr replace_substr(substr s, substr from, substr to)
{
  substr_iter iter = s;
  for (substr match = next_occurrence(&iter, from);
       !is_null_substr(match);
       match = next_occurrence(&iter, from)) {
    copy_substr(match, to);
  }
  return s; // FIXME
}

int main(void)
{
  do {
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
  } while(0);

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
    char *s = "foobar";
    // Empty strings do not overlap
    substr x = slice(s, 0, 0);
    substr y = slice(s, 0, 0);
    assert(!substr_overlaps(x, y));
  } while(0);
  do {
    char *s = "foobar";
    // Empty strings do not overlap
    substr x = slice(s, 0, 1);
    substr y = slice(s, 0, 0);
    assert(!substr_overlaps(x, y));
    assert(!substr_overlaps(y, x));
  } while(0);

  do {
    char *s = "foobar";
    substr x = slice(s, 0, 1);
    substr y = slice(s, 0, 1);
    assert(substr_overlaps(x, y));
    assert(substr_overlaps(y, x));
  } while(0);
  do {
    char *s = "foobar";
    substr x = slice(s, 0, 5);
    substr y = slice(s, 0, 5);
    assert(substr_overlaps(x, y));
    assert(substr_overlaps(y, x));
  } while(0);
  do {
    char *s = "foobar";
    substr x = slice(s, 0, 5);
    substr y = slice(s, 5, 10);
    assert(!substr_overlaps(x, y));
    assert(!substr_overlaps(y, x));
  } while(0);

  do {
    char *s = "foobar";
    substr x = slice(s, 1, 5);
    substr y = slice(s, 3, 10);
    assert(substr_overlaps(x, y));
    assert(substr_overlaps(y, x));
  } while(0);

  do {
    char *s = "foobar";
    substr x = slice(s, 1, 5);
    substr y = slice(s, 3, 10);
    assert(substr_left_overlaps(x, y));
    assert(!substr_left_overlaps(y, x));
  } while(0);

  do {
    char *s = "foobar";
    substr x = slice(s, 1, 5);
    substr y = slice(s, 1, 5);
    assert(is_subrange(x, y));
  } while(0);
  do {
    char *s = "foobar";
    substr x = slice(s, 3, 5);
    substr y = slice(s, 1, 5);
    assert(is_subrange(x, y));
  } while(0);
  do {
    char *s = "foobar";
    substr x = slice(s, 1, 3);
    substr y = slice(s, 1, 5);
    assert(is_subrange(x, y));
  } while(0);
  do {
    char *s = "foobar";
    substr x = slice(s, 0, 5);
    substr y = slice(s, 1, 5);
    assert(!is_subrange(x, y));
  } while(0);
  do {
    char *s = "foobar";
    substr x = slice(s, 1, 6);
    substr y = slice(s, 1, 5);
    assert(!is_subrange(x, y));
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

  printf("removing...\n");
  do {
    char x_buf[] = "foobar", z_buf[] = "foobar";
    substr x = as_substr(x_buf);
    substr z = as_substr(z_buf);
    substr y = slice(x_buf, 1, 3);

    print_substr(x); printf(" -> ");
    substr res = delete_substr(z, x, y);
    print_substr(res); printf(" # "); print_substr(z); printf("\n");
    
    print_substr(x); printf(" -> ");
    res = delete_substr_inplace(x, y);
    print_substr(res); printf(" # "); print_substr(x); printf("\n");
  } while(0);
  printf("\n");

  printf("iterating over words\n");
  char x[] = "\tfoo    bar123baz\nqux321";
  substr_iter iter = as_substr(x);
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
