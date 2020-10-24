#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

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


  struct link {
    struct refcount refcount;
    struct link * const next;
    int value;
  };

void dec_ref(struct refcount *rc)
{
  printf("decref %p (%d): %zu\n", (void *)rc,
  ((struct link *)rc)->value,
  rc->refs);
  if (rc->refs <= 1) rc->free(rc);
  else rc->refs--;
}

void inc_ref(struct refcount *rc)
{
  rc->refs++;
  printf("incref %p (%d): %zu\n", (void *)rc,
  ((struct link *)rc)->value,
  rc->refs);
}

#define DECREF(rc) do { if (rc) dec_ref((struct refcount *)rc); } while(0)
#define INCREF(rc) ((rc) ? (inc_ref((struct refcount *)rc), rc) : 0)



void free_link(struct link *x)
{
  printf("Deleting %p (%d)\n", (void *)x, x->value);
  DECREF(x->next);
  free(x);
}

struct link *new_link(int val, struct link *next)
{
  struct link *link = malloc(sizeof *link);
  if (!link) return 0;

  // Initialising while circumventing const of next
  struct link link_data = {
    .refcount = NEW_REFCOUNT(free_link),
    .next     = INCREF(next),
    .value    = val
  };
  memcpy(link, &link_data, sizeof *link);

  return link;
}

typedef struct link *list;

void free_list(list list)
{
  free_link(list);
}

list make_list(int n, int array[n])
{
  struct link *list = 0;
  for (int i = n - 1; i >= 0; i--) {
    struct link *link = new_link(array[i], list);
    if (!link) { // Allocation error -- clean up
      free_list(list);
      return 0;
    }
    list = link;
  }
  return list;
}

void print_list(list list)
{
  printf("[ ");
  while (list) {
      printf("%d ", list->value);
      list = list->next;
  }
  printf("]\n");
}


bool contains(list list, int val)
{
  while (list) {
      if (list->value == val)
          return true;
      list = list->next;
  }
  return false;
}

list prepend(list list, int val)
{
  struct link *new_list = new_link(val, list);
  if (!new_list) free_list(list);
  return new_list;
}

list append(list list, int val)
{
#if 0
  struct link *val_link = new_link(val, 0);
  if (!val_link) {
    free_list(list);
    return 0;
  }

  if (!list) return val_link;

  struct link *last = list;
  while (last->next) {
      last = last->next;
  }
  last->next = val_link;
#endif
  return list;
}

int main(int argc, char **argv)
{
  srand(time(0));
  int array[] = { 1, 2, 3, 4, 5 };
  int n = sizeof array / sizeof *array;

  struct link *list = make_list(n, array);
  if (!list) {
    perror("List error: ");
    exit(1); // Just bail here
  }

  printf("Contains:\n");
  printf("%d %d %d\n",
          contains(list, 0),
          contains(list, 3),
          contains(list, 6));
  free_list(list);
  printf("\n");


  return EXIT_SUCCESS;
}
