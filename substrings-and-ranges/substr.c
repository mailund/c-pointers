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

// Use a null begin pointer as a special symbol
static substr const NULL_SUBSTR = { .begin = 0 };
#define null_substr(x)  ((x).begin == 0)
#define empty_substr(x) ((x).begin == (x).end)
#define substr_len(x)   ((x).end - (x).begin)

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
  if (empty_substr(s)) return;
  char * restrict x = s.begin;
  char * restrict y = s.end - 1;
  for (; x < y; x++, y--) {
    char c = *x; *x = *y; *y = c; // swap
  }
}

// It is the caller's responsibility to check that
// x and y have the same length and do not overlap
void swap_substr(substr x, substr y)
{
  char * restrict p = x.begin;
  char * restrict q = y.begin;
  for (; p != x.end && q != y.end; p++, q++) {
    char c = *p; *p = *q; *q = c; // swap
  }
}


#define MIN(a,b) (((a)<(b)) ? (a) : (b))

// We write the charcters in from to to, but only
// part of from if to is shorter than from.
substr copy_substr(substr to, substr from)
{
  size_t n = MIN(substr_len(to), substr_len(from));
  // copy right cannot handle empty strings, so bail out here
  if (n == 0) return to;

  // Technically, the comparison is only well defined if the pointers
  // are into the same allocated buffer, but we are always allowed to
  // test, and we do not care about the result if they are not in the
  // same buffer, so it doesn't matter.
  if (to.begin < from.begin) { // copy left
      char * restrict y = to.begin;
      char * restrict x = from.begin, *xend = from.begin + n;
      while (x < xend) {
        *y++ = *x++;
      }
  } else { // copy right
    char * restrict y = to.begin + n;
    char * restrict x = from.begin + n, *xbeg = from.begin;
    do {
      *(--y) = *(--x);
    } while (x > xbeg);
  }

  return SUBSTR(to.begin + n, to.end);
}





// Remove substr y from substr x.
// It is the caller's responsibility to check that
// y is a subrange of x.
substr delete_substr(substr out, substr x, substr y)
{
  substr before = SUBSTR(x.begin, y.begin);
  substr after = SUBSTR(y.end, x.end);
  substr tmp = copy_substr(out, before);
  tmp = copy_substr(tmp, after);
  return SUBSTR(out.begin, tmp.begin);
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

// Replace string x in z with the string in y.
// It is the caller's responsibility to ensure that
// x is a subrange of z and y is not contained
// in out.
substr replace_substr(substr out,
                      substr z, substr x,
                      substr y)
{
  substr tmp = out;
  tmp = copy_substr(tmp, SUBSTR(z.begin, x.begin));
  tmp = copy_substr(tmp, y);
  tmp = copy_substr(tmp, SUBSTR(x.end, z.end));
  return SUBSTR(out.begin, tmp.begin);
}

// It is the caller's responsibility to ensure that
// x is a subrange of z and that y is not contained
// in z.
substr replace_substr_inplace(substr z, substr x,
                              substr y)
{
  substr after = SUBSTR(x.end, z.end);
  char *x_end = MIN(x.begin + substr_len(y), z.end);
  substr after_dest = SUBSTR(x_end, z.end);

  char *copy_end = copy_substr(after_dest, after).begin;
  copy_substr(SUBSTR(x.begin, x_end), y);

  return SUBSTR(z.begin, copy_end);
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
       !null_substr(word);
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

// Iterator for non-overlapping occurrences of s
substr next_occurrence(substr_iter *iter, substr s)
{
  substr occ = find_occurrence(*iter, s);
  if (!null_substr(occ)) {
    // if there was an occurrence, we must update
    // the iterator
    iter->begin = occ.end;
  }
  return occ;
}

// FIXME: handle unequal length
substr replace_all_occurrences(substr s, substr from, substr to)
{
  substr_iter iter = s;
  for (substr match = next_occurrence(&iter, from);
       !null_substr(match);
       match = next_occurrence(&iter, from)) {
    copy_substr(match, to);
  }
  return s; // FIXME
}
