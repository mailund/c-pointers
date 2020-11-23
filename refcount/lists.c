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
struct link NIL_LINK = { .refcount = 1 };
#define is_nil(x)   ((x) == &NIL_LINK)
#define is_error(x) ((x) == 0)

#define takes
#define borrows

#define give(x) x
#define share(x) incref(x)

#define NIL incref(&NIL_LINK)

list cons(int head, takes list tail)
{
  if (is_error(tail)) return tail;

  list new_link = malloc(sizeof *new_link);
  if (!new_link) { decref(tail); return 0; }

  links++;
  struct link link_data = {
    .refcount = 1,
    .next  = tail, // gives away the reference
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
    return length_rec(next, acc + 1);
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
    return acc;
  } else {
    int value = x->value;
    struct link *next = incref(x->next);
    decref(x);
    return reverse_rec(next, cons(value, acc));
  }
}

#define reverse(x) reverse_rec(x, NIL)


#endif


int main(void)
{
  printf("CONSTRUCTION\n");
  list x = cons(1, cons(2, cons(3, NIL)));
  print_list(x);

  list y = cons(0, share(x));
  print_list(y);

  decref(x);
  print_list(y);
  decref(y);

  printf("\n%d links left\n\n", links);

  printf("LENGTH\n");
  x = cons(1, cons(2, cons(3, NIL)));
  print_list(x);
  printf("len(x) == %d\n", length(share(x)));
  print_list(x);
  // leaks if length doesn't free its input
  printf("len(x) == %d\n", length(cons(1, cons(2, cons(3, NIL)))));

  //decref(x);
  // this frees x
  length(give(x));

  printf("\n%d links left\n\n", links);

  printf("REVERSE\n");

  x = cons(1, cons(2, cons(3, NIL)));
  y = reverse(x);
  print_list(y);
  decref(x);
  decref(y);

  // would leak if reverse and length didn't free their input
  printf("%d\n", length(reverse(cons(1, cons(2, cons(3, NIL))))));

  printf("\n%d links left\n\n", links);

  return 0;
}
