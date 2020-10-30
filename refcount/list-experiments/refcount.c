
#include <stdlib.h>
#include <stdio.h>

typedef void (*deallocator)(void *);
struct refcount {
  int count;
  deallocator free;
};

#define NEW_REFCOUNT(deallocator) \
  (struct refcount){ .count = 1, .free = (deallocator) }

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

struct link {
  struct refcount rc;
  struct link *next;
  int value;
};
typedef struct link *list;

void free_list(list x);

list cons(int car, list cdr)
{
  list new_link   = malloc(sizeof *new_link);
  new_link->rc    = NEW_REFCOUNT((deallocator)free_list);
  new_link->next  = incref(cdr);
  new_link->value = car;
  return new_link;
}

void print_list(list x)
{
  printf("[");
  while (x) {
    printf("%d ", x->value);
    x = x->next;
  }
  printf("]\n");
}

void free_list(list x)
{
  printf("freeing: ");
  print_list(x);
  decref(x->next);
  free(x);
}

int main(void)
{
  list x = cons(1, cons(2, cons(3, 0)));
  decref(x);

  return 0;
}
