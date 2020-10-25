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
  if (!rc || rc->free == 0)          return rc;
  if (rc->refs > 1)  { rc->refs--;   return rc; }
  else               { rc->free(rc); return 0;  }
}

// return void to avoid explicit casting
void *inc_ref(struct refcount *rc)
{
  if (rc) rc->refs++;
  return rc;
}

void *protect(struct refcount *rc)
{
  assert(rc && rc->refs > 0);
  rc->refs++;
  return rc;
}

void *unprotect(struct refcount *rc)
{
  assert(rc && rc->refs > 0);
  rc->refs--;
  return rc;
}

#define DECREF(rc)    dec_ref(&(rc)->refcount)
#define INCREF(rc)    inc_ref(&(rc)->refcount)
#define PROTECT(rc)   protect(&(rc)->refcount)
#define UNPROTECT(rc) unprotect(&(rc)->refcount)

struct link {
  struct refcount refcount;
  struct link * const next;
  int value;
};

struct link NIL_LINK = { .refcount = PERSISTENT_REF };
#define NIL (&NIL_LINK)
#define empty_list(x) (x == NIL)

#define ERROR_LIST 0
#define error_list(x) (x == ERROR_LIST)

size_t allocated_links; // for debugging

// FIXME: recursion depth?
void free_link(struct link *x)
{
  DECREF(x->next);
  free(x);
  allocated_links--;
}


// The incref/decref pattern frees with errors...
struct link *new_link(int val, struct link *next)
{
  if (!next) return 0;

  INCREF(next); // always incref input

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

  // always decref again when done
  DECREF(next);

  return link;
}

// FIXME: typechecking disabled with void * :(
#define REFCOUNT_VARS(...) \
  void *_refcount_vars_[] = { __VA_ARGS__ };

#define REFCOUNT_ALL_VARS(func)              \
  for (int i = 0;                            \
       i < sizeof _refcount_vars_ /          \
           sizeof *_refcount_vars_;          \
       i++) {                                \
    func(_refcount_vars_[i]);                \
  }                                          \

#define INCREF_VARS()                        \
  REFCOUNT_ALL_VARS(inc_ref)
#define DECREF_VARS()                        \
  REFCOUNT_ALL_VARS(dec_ref)

#define NULL_REF_ERROR(x)                    \
  if (!x) goto _refcount_error_;
#define CHECK_INPUT()                        \
  REFCOUNT_ALL_VARS(NULL_REF_ERROR)

// We cannot put ENTER in do-while because we need
// the variables to be in the function scope...
#define ENTER(...)                           \
    REFCOUNT_VARS(__VA_ARGS__);              \
    INCREF_VARS();                           \
    CHECK_INPUT();
#define RETURN(expr)                         \
  do {                                       \
    DECREF_VARS();                           \
    return (expr);                           \
  } while(0)
#define ERROR_BLOCK()                        \
  do {                                       \
_refcount_error_:                            \
    DECREF_VARS();                           \
    return 0;                                \
  } while(0)


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

void print_refcounts(list x)
{
  printf("[");
  while (!empty_list(x)) {
    printf("%d <%zu> ", x->value, x->refcount.refs);
    x = x->next;
  }
  printf("]\n");
}

bool contains(list x, int val)
{
  ENTER(x);

  if (empty_list(x))   RETURN(false);
  if (x->value == val) RETURN(true);
  list next = PROTECT(x->next);
  RETURN(contains(UNPROTECT(next), val));

  ERROR_BLOCK();
}

list reverse(list x, list acc)
{
  ENTER(x, acc);

  if (empty_list(x)) {
    PROTECT(acc);
    RETURN(UNPROTECT(acc));

  } else {
    int value = x->value;
    struct link *next = PROTECT(x->next);
    PROTECT(acc);
    RETURN(reverse(UNPROTECT(next),
                   new_link(value, UNPROTECT(acc))));
  }

  ERROR_BLOCK();
}

list concat(list x, list y, list acc)
{
  ENTER(x, y, acc);

  if (empty_list(x)) {
    PROTECT(y);
    RETURN(UNPROTECT(y));
  } else {
    int value = x->value;
    struct link *next = PROTECT(x->next);
    PROTECT(y); PROTECT(acc);
    RETURN(new_link(value,
           concat(UNPROTECT(next), UNPROTECT(y), UNPROTECT(acc))));
  }

  ERROR_BLOCK();
}

// Leaks with error
list prepend_list(list x, list acc)
{
  ENTER(x, acc);

  if (empty_list(x)) {
    PROTECT(acc);
    RETURN(UNPROTECT(acc));

  } else {
    int value = x->value;
    struct link *next = PROTECT(x->next);
    PROTECT(acc);
    RETURN(prepend_list(UNPROTECT(next),
                        new_link(value, UNPROTECT(acc))));
  }

  ERROR_BLOCK();
}
list concat_tr(list x, list y, list acc)
{
  ENTER(x, y, acc);

  if (empty_list(x)) {
    PROTECT(acc); PROTECT(y);
    RETURN(prepend_list(UNPROTECT(acc), UNPROTECT(y)));
  } else {
    int value = x->value;
    struct link *next = PROTECT(x->next);
    PROTECT(y); PROTECT(acc);
    RETURN(concat_tr(UNPROTECT(next),
                     UNPROTECT(y),
                     new_link(value, UNPROTECT(acc))));
  }  

  ERROR_BLOCK();
}


int main(int argc, char **argv)
{
  srand(time(0));
  int array[] = { 1, 2, 3, 4, 5 };
  int n = sizeof array / sizeof *array;

  list x = INCREF(make_list(n, array));
  if (error_list(x)) {
    perror("List error: ");
    exit(1); // Just bail here
  }
  printf("x ref count: %zu\n", x->refcount.refs);
  printf("x: ");
  print_refcounts(x);

  printf("Contains:\n");
  printf("%d %d %d\n",
          contains(x, 0),
          contains(x, 3),
          contains(x, 6));
  free_list(x);
  printf("\n");
  printf("allocated links: %zu\n", allocated_links);
  printf("\n");

  x = INCREF(reverse(make_list(n, array), NIL));
  printf("x ref count: %zu\n", x->refcount.refs);
  printf("x: ");
  print_refcounts(x);
  free_list(x);
  printf("allocated links: %zu\n", allocated_links);
  printf("\n");

  x = INCREF(make_list(n, array));
  printf("with x, allocated links: %zu\n", allocated_links);
  printf("\n");
  list y = INCREF(new_link(10, new_link(11, NIL)));
  printf("wit y, allocated links: %zu\n", allocated_links);
  printf("\n");

  printf("concatenating\n");
  list z = INCREF(concat(x, y, NIL));
  printf("with z, allocated links: %zu\n", allocated_links);
  printf("\n");

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
