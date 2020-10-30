#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef void (*deallocator)(void *);
struct refcount {
  int count;
  deallocator free;
};

void *incref(void *p)
{
  struct refcount *rc = p;
  if (rc) rc->count++;
  return rc;
}

void *decref(void *p)
{
  struct refcount *rc = p;
  if (rc && --(rc->count) < 1) {
    rc->free(rc);
    return 0;
  }
  return rc;
}

void *protect(void *p)
{
  struct refcount *rc = p;
  assert(rc);
  rc->count++;
  return rc;
}

void *unprotect(void *p)
{
  struct refcount *rc = p;
  assert(rc && rc->count > 0);
  rc->count--;
  return rc;
}


#define REFCOUNT_VARS(...) \
  void *_refcount_vars_[] = { __VA_ARGS__ };

#define REFCOUNT_ALL_VARS(func)              \
  do {                                       \
    for (int i = 0;                          \
         i < sizeof _refcount_vars_ /        \
             sizeof *_refcount_vars_;        \
         i++) {                              \
      func(_refcount_vars_[i]);              \
    }                                        \
  } while(0)

#define INCREF_VARS()                        \
  REFCOUNT_ALL_VARS(incref)
#define DECREF_VARS()                        \
  REFCOUNT_ALL_VARS(decref)

#define CHECK_INPUT(func)                    \
  do {                                       \
    for (int i = 0;                          \
         i < sizeof _refcount_vars_ /        \
             sizeof *_refcount_vars_;        \
         i++) {                              \
      if (!_refcount_vars_[i]) {             \
        DECREF_VARS();                       \
        return 0;                            \
      }                                      \
    }                                        \
  } while(0)


#define ENTER(...)                           \
    REFCOUNT_VARS(__VA_ARGS__);              \
    INCREF_VARS();                           \
    CHECK_INPUT();

#define REFCOUNT_ALL_PROTECTED(func)         \
  do {                                       \
    for (int i = 0;                          \
         i < sizeof _refcount_protected_ /   \
             sizeof *_refcount_protected_;   \
         i++) {                              \
      func(_refcount_protected_[i]);         \
    }                                        \
  } while(0)

#define PROTECTED_EXIT(...)                          \
  do {                                               \
    void *_refcount_protected_[] = { __VA_ARGS__ };  \
    REFCOUNT_ALL_PROTECTED(protect);                 \
    DECREF_VARS();                                   \
    REFCOUNT_ALL_PROTECTED(unprotect);               \
  } while(0)


#define RETURN(expr) \
  do { DECREF_VARS(); return (expr); } while(0)
#define RETURN_PROTECTED(expr, ...) \
  do { PROTECTED_EXIT(__VA_ARGS__); return (expr); } while(0)


struct link {
  struct refcount     rc;
  struct link * const next;
  int           const value;
};

typedef struct link *list;

struct link NIL_LINK = { .rc = { .count = 1 } };
#define NIL (&NIL_LINK)
#define is_nil(x)   ((x) == NIL)
#define is_error(x) ((x) == 0)

int links;

void print_list(list x);
void free_list(list x)
{
  printf("freeing: ");
  print_list(x);
  decref(x->next);
  free(x);
  links--;
  printf("NIL has %d references\n", NIL_LINK.rc.count);
  printf("now there are %d links\n", links);
}

list cons(int car, list cdr)
{
  if (is_error(cdr)) return cdr;
  incref(cdr);
  list new_link = malloc(sizeof *new_link);
  if (new_link) {
    links++;
    struct link link_data = {
      .rc = (struct refcount){
        .count = 0,
        .free  = (deallocator)free_list
      },
      .next  = incref(cdr),
      .value = car
    };
    memcpy(new_link, &link_data, sizeof *new_link);
  }
  decref(cdr);
  return new_link;
}

int length(list x, int acc)
{
  ENTER(x);

  if (is_nil(x)) RETURN(acc);
  else {
    list next = x->next;
    RETURN_PROTECTED(
      length(next, acc + 1),
      next
    );
  }
}

list reverse(list x, list acc)
{
  ENTER(x, acc);
  if (is_nil(x)) {
    RETURN_PROTECTED(acc, acc);
  } else {
    list next = x->next;
    int value = x->value;
    RETURN_PROTECTED(
      reverse(next, cons(value, acc)),
      next, acc
    );
  }
}

list concat(list x, list y)
{
  ENTER(x, y);
  if (is_nil(x)) {
    RETURN_PROTECTED(y, y);
  } else {
    list next = x->next;
    int value = x->value;
    RETURN_PROTECTED(
      cons(value, concat(next, y)),
      next, y
    );
  }
}

list prepend_list(list x, list acc)
{
  ENTER(x, acc);

  if (is_nil(x)) {
    RETURN_PROTECTED(acc, acc);
  } else {
    list next = x->next;
    int value = x->value;
    RETURN_PROTECTED(
      prepend_list(next, cons(value, acc)),
      next, acc
    );
  }
}
list concat_tr(list x, list y, list acc)
{
  ENTER(x, y, acc);
  if (is_nil(x)) {
    RETURN_PROTECTED(
      prepend_list(acc, y),
      acc, y
    );
  } else {
    list next = x->next;
    int value = x->value;
    RETURN_PROTECTED(
      concat_tr(next, y, cons(value, acc)),
      next, y, acc
    );
  }
}


void print_list(list x)
{
  printf("[ ");
  while (!is_nil(x)) {
    printf("%d[%d] ", x->value, x->rc.count);
    x = x->next;
  }
  printf("]\n");
}

int main(void)
{
  list x = incref(cons(1, cons(2, cons(3, incref(NIL)))));
  printf("x: "); print_list(x);
  printf("NIL has %d references\n", NIL_LINK.rc.count);
  printf("There are %d links\n", links);
  printf("x has length %d\n", length(x, 0));
  printf("There are %d links\n", links);

  list y = incref(reverse(x, NIL));
  printf("y: "); print_list(y);
  printf("There are %d links\n", links);
  printf("y has length %d\n", length(y, 0));
  printf("There are %d links\n", links);

  list z = incref(concat(x, y));

  printf("z has length %d\n", length(z, 0));
  printf("There are %d links\n", links);
  list w = incref(concat_tr(x, y, NIL));

  printf("x: "); print_list(x);
  printf("y: "); print_list(y);
  printf("z: "); print_list(z);
  printf("w: "); print_list(w);

  decref(x); decref(y); decref(z); decref(w);

  printf("There are %d links\n", links);

  return 0;
}
