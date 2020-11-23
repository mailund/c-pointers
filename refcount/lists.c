#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

struct link {
  int refcount;
  struct link * const next;
  int           const value;
};
typedef struct link *list;

struct link *incref(struct link *link)
{
  link->refcount++;
  return link;
}

void free_link(struct link *link);
struct link *decref(struct link *link)
{
  if (link && --link->refcount == 0) {
    free_link(link);
    return 0;
  }
  return link;
}

#warning debugging info
int links;

void free_link(struct link *link)
{
  decref(link->next);
  free(link);
  links--;
}

// Empty lists and errors
#define is_error(x) ((x) == 0)
struct link NIL_LINK = { .refcount = 1 };
#define is_nil(x)   ((x) == &NIL_LINK)
#define NIL incref(&NIL_LINK)

// Give a reference; you lose it yourself
#define takes
#define give(x) x

#if 0
// GCC compiler extension statement-expression for give()
#define give(x)  ({ void *p = x; x = 0; p; })
#endif

// Let the function use your reference
#define borrows


list cons(int head, takes list tail)
{
  if (is_error(tail)) return tail;

  list new_link = malloc(sizeof *new_link);
  if (!new_link) { decref(tail); return 0; }

#warning debug info
  links++;
  struct link link_data = {
    .refcount = 1,
    .next  = give(tail), // gives away the reference
    .value = head
  };
  memcpy(new_link, &link_data, sizeof *new_link);
  return new_link;
}

void print_list(borrows list x)
{
  printf("[ ");
  while (!is_nil(x)) {
    printf("%d[%d] ", x->value, x->refcount);
    x = x->next;
  }
  printf("]\n");
}

// LENGTH ------------------------------------------
#if 0
int length_rec(borrows list x, int acc)
{
  assert(!is_error(x));
  if (is_nil(x))
    return acc;
  else
    return length_rec(x->next, acc + 1);
}
#define length(x) length_rec(x, 0)

#else

int length_rec(takes list x, int acc)
{
  assert(!is_error(x));
  if (is_nil(x)) {
    decref(x);
    return acc;
  } else {
    struct link *next = incref(x->next);
    decref(x);
    return length_rec(give(next), acc + 1);
  }
}
#define length(x) length_rec(x, 0)

#endif

// REVERSE --------------------------------------
#if 0
list reverse_rec(borrows list x, borrows list acc)
{
  if (is_error(x) || is_error(acc)) {
    return 0;
  }
  if (is_nil(x)) {
    return acc;
  } else {
    return reverse_rec(x->next, cons(x->value, acc));
  }
}

#define reverse(x) reverse_rec(x, NIL)

#else

list reverse_rec(takes list x, takes list acc)
{
  if (is_error(x) || is_error(acc)) {
    decref(x); decref(acc);
    return 0;
  }
  if (is_nil(x)) {
    decref(x);
    return give(acc);

  } else {
    int value = x->value;
    struct link *next = incref(x->next);
    decref(x);
    return reverse_rec(give(next), cons(value, acc));
  }
}

#define reverse(x) reverse_rec(x, NIL)

#endif

// CONCAT --------------------------------------
#if 0
list concat(borrows list x, borrows list y)
{
  if (is_error(x) || is_error(y)) {
    return 0;
  }
  if (is_nil(x)) {
    return incref(y); // we return a new ref, so we must incref here
  } else {
    return cons(x->value, concat(x->next, y));
  }
}

#else

list concat(takes list x, takes list y)
{
  if (is_error(x) || is_error(y)) {
    decref(x); decref(y);
    return 0;
  }
  if (is_nil(x)) {
    return give(y);
  } else {
    int value = x->value;
    struct link *next = incref(x->next);
    decref(x);
    return cons(value, concat(give(next), give(y)));
  }
}


#endif


int main(void)
{
  printf("CONSTRUCTION\n");
  list x = cons(1, cons(2, cons(3, NIL)));
  print_list(x);

  list y = cons(0, incref(x));
  print_list(y);

  decref(x);
  print_list(y);

  list z = cons(-1, give(y)); // consider y gone!
  print_list(z);
  decref(z);

  printf("\n%d links left\n\n", links);

  printf("LENGTH\n");
  x = cons(1, cons(2, cons(3, NIL)));
  print_list(x);
  printf("len(x) == %d\n", length(incref(x)));
  print_list(x);
  // leaks if length doesn't free its input
  printf("len(x) == %d\n", length(cons(1, cons(2, cons(3, NIL)))));

  //decref(x);
  // this frees x
  length(give(x));

  printf("\n%d links left\n\n", links);

  printf("REVERSE\n");

  x = cons(1, cons(2, cons(3, NIL)));
  print_list(x);
  y = reverse(give(x));
  print_list(y);
  decref(y);

  // would leak if reverse and length didn't free their input
  printf("%d\n", length(reverse(cons(1, cons(2, cons(3, NIL))))));

  printf("\n%d links left\n\n", links);

  printf("CONCAT\n");
  x = cons(1, cons(2, cons(3, NIL)));
  y = cons(4, cons(5, NIL));
  z = concat(incref(x), incref(y));

  print_list(z);
  decref(x); decref(y);
  print_list(z);
  decref(z);

  z = concat(cons(1, cons(2, NIL)), cons(11, cons(12, cons(13, NIL))));
  print_list(z);
  decref(z);

  printf("\n%d links left\n\n", links);

  return 0;
}
