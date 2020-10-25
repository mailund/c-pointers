#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#if 0
// fake malloc
void * (*ml)(size_t) = malloc;
void *fake_malloc(size_t size)
{
  if (rand() % 10 == 0) {
    return 0;
  } else {
    return ml(size);
  }
}
#define malloc fake_malloc
#endif

typedef void (*deallocator)(void *);

struct refcount {
  size_t refs;
  deallocator free;
};

#define NEW_REFCOUNT(free_func)          \
  (struct refcount){                     \
    .refs = 0,                           \
    .free = (deallocator)(free_func)     \
  }
#define PERSISTENT_REF  \
  (struct refcount){ .refs = 0, .free = 0 }


void *dec_ref(struct refcount *rc)
{
  if (!rc)           return 0;
  if (rc->free == 0) return rc;
  if (rc->refs > 1)  { rc->refs--;   return rc; }
  else               { rc->free(rc); return 0;  }
}


// return void to avoid explicit casting
void *inc_ref(struct refcount *rc)
{
  if (rc) rc->refs++;
  return rc;
}

#define DECREF(rc) dec_ref(&(rc)->refcount)
#define INCREF(rc) inc_ref(&(rc)->refcount)

struct link {
  struct refcount refcount;
  struct link * const next;
  int value;
};

struct link NIL_LINK = { .refcount = PERSISTENT_REF };
#define NIL (&NIL_LINK)
#define empty_list(x) (x == NIL)


size_t allocated_links; // for debugging

// FIXME: recursion depth?
void free_link(struct link *x)
{
  DECREF(x->next);
  free(x);
  allocated_links--;
}

// This is a leaking version in case of errors!
struct link *new_link(int val, struct link *next)
{
  if (!next) return 0;

  struct link *link = malloc(sizeof *link);
  if (link) {
    allocated_links++;
    // Initialising while circumventing const of next
    struct link link_data = {
      .refcount = NEW_REFCOUNT(free_link),
      .next     = INCREF(next),
      .value    = val
    };
    memcpy(link, &link_data, sizeof *link);
  }

  return link;
}


typedef struct link *list;

#define free_list(x) \
  do { DECREF(x); x = NIL; } while(0)

list make_list(int n, int array[n])
{
  list x = NIL;
  for (int i = n - 1; i >= 0; i--) {
    x = new_link(array[i], x);
  }
  return x;
}

void print_list(list x)
{
  printf("[ ");
  while (!empty_list(x)) {
      printf("%d ", x->value);
      x = x->next;
  }
  printf("]\n");
}

bool contains(list x, int val)
{
  if (empty_list(x))   return false;
  if (x->value == val) return true;
  return contains(x->next, val);
}

list reverse(list x, list acc)
{
  if (empty_list(x)) return acc;
  else return reverse(x->next, new_link(x->value, acc));
}

list concat(list x, list y, list acc)
{
  if (empty_list(x)) return y;
  else return new_link(x->value, concat(x->next, y, acc));
}

// Leaks with error
list prepend_list(list x, list acc)
{
  if (empty_list(x)) return acc;
  else return prepend_list(x->next, new_link(x->value, acc));
}
list concat_tr(list x, list y, list acc)
{
  if (empty_list(x)) return prepend_list(acc, y);
  else return concat_tr(x->next, y, new_link(x->value, acc));
}

void print_refcounts(list x)
{
  printf("[");
  while (!empty_list(x)) {
    printf("%d <%zu> ", x->value, x->refcount.refs);
    x = x->next;
  }
  printf("]\n");
}

int main(int argc, char **argv)
{
  srand(time(0));
  int array[] = { 1, 2, 3, 4, 5 };
  int n = sizeof array / sizeof *array;

  list x = INCREF(make_list(n, array));
  if (!x) {
    perror("List error: ");
    exit(1); // Just bail here
  }
  printf("x ref count: %zu\n", x->refcount.refs);

  printf("Contains:\n");
  printf("%d %d %d\n",
          contains(x, 0),
          contains(x, 3),
          contains(x, 6));
  free_list(x);
  printf("\n");

  x = INCREF(make_list(n, array));
  list y = INCREF(new_link(-1, new_link(0, NIL)));
  INCREF(y);
  printf("y ref count: %zu\n", y->refcount.refs);

  printf("x: ");
  print_refcounts(x);
  printf("y: ");
  print_refcounts(y);

  printf("reversing\n");
  list z = INCREF(reverse(x, NIL));

  printf("x: ");
  print_refcounts(x);
  printf("y: ");
  print_refcounts(y);
  printf("z: ");
  print_refcounts(z);

  printf("z ref count: %zu\n", z->refcount.refs);
  printf("free z");
  free_list(z);
  printf("z: ");
  print_refcounts(z);
  printf("\n");

  printf("concatenating\n");
  z = INCREF(concat(x, y, NIL));

  printf("x: ");
  print_refcounts(x);
  printf("y: ");
  print_refcounts(y);
  printf("z: ");
  print_refcounts(z);

  printf("free x\n");
  free_list(x);
  printf("\n");

  printf("x: ");
  print_refcounts(x);
  printf("y: ");
  print_refcounts(y);
  printf("z: ");
  print_refcounts(z);
  printf("\n");

  printf("free y\n");
  free_list(y);
  printf("\n");

  printf("x: ");
  print_refcounts(x);
  printf("y: ");
  print_refcounts(y);
  printf("z: ");
  print_refcounts(z);
  printf("\n");

  printf("free z\n");
  free_list(z);
  printf("\n");

  printf("x: ");
  print_refcounts(x);
  printf("y: ");
  print_refcounts(y);
  printf("z: ");
  print_refcounts(z);
  printf("\n");

  printf("concatenating tr\n");
  x = INCREF(new_link(1, new_link(2, NIL)));
  y = INCREF(new_link(3, new_link(4, new_link(5, NIL))));
  z = INCREF(concat(x, y, NIL));

  printf("x: ");
  print_refcounts(x);
  printf("y: ");
  print_refcounts(y);
  printf("z: ");
  print_refcounts(z);

  printf("free x\n");
  free_list(x);
  printf("\n");

  printf("x: ");
  print_refcounts(x);
  printf("y: ");
  print_refcounts(y);
  printf("z: ");
  print_refcounts(z);
  printf("\n");

  printf("free y\n");
  free_list(y);
  printf("\n");

  printf("x: ");
  print_refcounts(x);
  printf("y: ");
  print_refcounts(y);
  printf("z: ");
  print_refcounts(z);
  printf("\n");

  printf("free z\n");
  free_list(z);
  printf("\n");

  printf("x: ");
  print_refcounts(x);
  printf("y: ");
  print_refcounts(y);
  printf("z: ");
  print_refcounts(z);
  printf("\n");


  printf("allocated links: %zu\n", allocated_links);

  return EXIT_SUCCESS;
}
