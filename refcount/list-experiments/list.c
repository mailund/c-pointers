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
  assert(!is_error(x));
  incref(x);
  if (is_nil(x)) {
    decref(x);
    return acc;
  }
  else {
    list next = protect(x->next);
    decref(x);
    return length(unprotect(next), acc + 1);
  }
}

list reverse(list x, list acc)
{
  incref(x); incref(acc);
  if (is_error(x) || is_error(acc)) {
    decref(x); decref(acc);
    return 0;
  }

  if (is_nil(x)) {
    decref(x);
    return unprotect(acc);
  } else {
    list next = protect(x->next);
    int value = x->value;
    decref(x);
    return reverse(unprotect(next),
                   cons(value, unprotect(acc)));
  }
}

list concat(list x, list y)
{
  incref(x); incref(y);
  if (is_error(x) || is_error(y)) {
    decref(x); decref(y);
    return 0;
  }

  if (is_nil(x)) {
    decref(x);
    return unprotect(y);
  }else {
    list next = protect(x->next);
    int value = x->value;
    decref(x);
    return cons(value,
                concat(unprotect(next),
                       unprotect(y)));
  }
}

list prepend_list(list x, list acc)
{
  incref(x); incref(acc);
  if (is_error(x) || is_error(acc)) {
    decref(x); decref(acc);
    return 0;
  }

  if (is_nil(x)) {
    decref(x);
    return unprotect(acc);
  } else {
    list next = protect(x->next);
    int value = x->value;
    decref(x);
    return prepend_list(unprotect(next),
                        cons(value, unprotect(acc)));
  }
}
list concat_tr(list x, list y, list acc)
{
  incref(x); incref(y); incref(acc);
  if (is_error(x) || is_error(y) || is_error(acc)) {
    decref(x); decref(y); decref(acc);
    return 0;
  }

  if (is_nil(x)) {
    decref(x);
    return prepend_list(unprotect(acc), unprotect(y));
  } else {
    list next = protect(x->next);
    int value = x->value;
    decref(x);
    return concat_tr(unprotect(next),
                     unprotect(y),
                     cons(value, unprotect(acc)));
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
