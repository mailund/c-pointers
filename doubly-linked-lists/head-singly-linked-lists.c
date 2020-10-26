#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <stdbool.h>

// fake malloc
void * (*ml)(size_t) = malloc;
void *fake_malloc(size_t size)
{
  if (rand() % 100000 == 0) {
    return 0;
  } else {
    return ml(size);
  }
}
#define malloc fake_malloc


struct link {
  int value;
  struct link *next;
};
typedef struct link list;
typedef list * list_ref;
#define empty_list ((list){ .next = 0 })

struct link *new_link(int val, struct link *next)
{
  struct link *link = malloc(sizeof *link);
  if (!link) return 0;
  link->value = val;
  link->next = next;
  return link;
}

void free_links(struct link *x)
{
  while (x) {
    // Remember next, we cannot get it
    // after free(list)
    struct link *next = x->next;
    free(x);
    x = next;
  }
}

#define free_list(x)       \
  do {                     \
    free_links((x).next);  \
    x = empty_list;        \
  } while(0)


// FIXME: error handling
list make_list(int n, int array[n])
{
  list x = empty_list;
  for (int i = n - 1; i >= 0; i--) {
    struct link *link = new_link(array[i], x.next);
    if (!link) { // Allocation error -- clean up
      free_list(x);
      return empty_list; // FIXME: error handling
    }
    x.next = link;
  }
  return x;
}

void print_list(list_ref x)
{
  printf("[ ");
  struct link *link = x->next;
  while (link) {
    printf("%d ", link->value);
    link = link->next;
  }
  printf("]\n");
}

bool contains(list_ref x, int val)
{
  struct link *link = x->next;
  while (link) {
    if (link->value == val)
      return true;
    link = link->next;
  }
  return false;
}

int prepend(list_ref x, int val)
{
  struct link *link = new_link(val, x->next);
  if (!link) return 0;
  x->next = link;
  return 1;
}

struct link *last_link(list *x)
{
  // When we start from x, there is always
  // a link where we can get the next
  struct link *prev = x;
  while (prev->next) {
    prev = prev->next;
  }
  return prev;
}

int append(list_ref x, int val)
{
  struct link *link = new_link(val, 0);
  if (!link) return 0;
  struct link *last = last_link(x);
  last->next = link;
  return 1;
}

void concatenate(list_ref x, list_ref y)
{
  struct link *last = last_link(x);
  last->next = y->next;
  *y = empty_list;
}

void delete_value(list_ref x, int val)
{
  // When we start from x, there is always
  // a node with a next pointer.
  struct link *prev = x;
  struct link *link = x->next;
  while (link) {
    if (link->value != val) {
      // Move to next...
      prev = link;
      link = link->next;
    } else {
      // Delete and move to next
      struct link *next = link->next;
      free(link);
      prev->next = link = next;
    }
  }
}

void reverse(list_ref x)
{
  // If x's next pointer is null, then it is
  // an empty list which is already empty.
  if (!x->next) return;

  // We have at least one link, so we can also
  // get the first link's next (which may or may
  // not be null).
  struct link *next = x->next->next;
  struct link *reversed = x->next;
  reversed->next = 0;
  while (next) {
      struct link *next_next = next->next;
      next->next = reversed;
      reversed = next;
      next = next_next;
  }
  x->next = reversed;
}

int main(void)
{
  srand(time(0));
  int array[] = { 1, 2, 3, 4, 5 };
  int n = sizeof(array) / sizeof(array[0]);

  list x = make_list(n, array);
  // FIXME: error handling?

  printf("Contains:\n");
  printf("%d %d %d\n",
          contains(&x, 0),
          contains(&x, 3),
          contains(&x, 6));
  free_list(x);
  printf("\n");

  printf("prepend/append\n");
  x = make_list(n, array);

  int success = append(&x, 6);
  if (!success) {
    perror("List error:");
    exit(1);
  }
  success = prepend(&x, 0);
  if (!success) {
    perror("List error:");
    exit(1);
  }
  print_list(&x);
  free_list(x);
  printf("\n");

  printf("concatenate:\n");
  x = make_list(n, array);
  list y = make_list(n, array);

  concatenate(&x, &y);
  print_list(&x);
  free_list(x);
  assert(x.next == 0);
  assert(y.next == 0);
  free_list(y);
  printf("\n");

  printf("deleting values:\n");
  x = make_list(n, array);
  print_list(&x);
  delete_value(&x, 2);
  delete_value(&x, 3);
  print_list(&x);
  free_list(x);
  printf("\n");

  printf("deleting first link:\n");
  x = make_list(n, array);
  delete_value(&x, 1);
  print_list(&x);
  free_list(x);
  printf("\n");

  printf("reversing:\n");
  x = make_list(n, array);
  reverse(&x);
  print_list(&x);
  free_list(x);
  printf("\n");

  return 0;
}
