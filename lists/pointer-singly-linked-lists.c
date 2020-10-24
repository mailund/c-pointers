#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// fake malloc
void * (*ml)(size_t) = malloc;
void *fake_malloc(size_t size)
{
  if (rand() % 100 == 0) {
    return 0;
  } else {
    return ml(size);
  }
}
#define malloc fake_malloc

// for contains
#include <stdbool.h>

struct link {
  int value;
  struct link *next;
};
// A list is a pointer to a link
typedef struct link * list;
typedef list * list_ref;

#define empty_list 0
#define is_list_empty(x) x
#define is_list_ref_empty(x) *(x)
#define list_ref_links(x) *(x)

struct link *new_link(int val, struct link *next)
{
  struct link *link = malloc(sizeof *link);
  if (!link) return 0;
  link->value = val;
  link->next = next;
  return link;
}

void free_links(struct link *list)
{
  while (list) {
      struct link *next = list->next;
      free(list);
      list = next;
  }
}

#define free_list(x)    \
  do {                  \
    free_links(x);      \
    x = empty_list;     \
  } while(0)

#define free_list_ref(x) free_list(*(x))

// Same as before...
struct link *make_list(int n, int array[n])
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

void print_list(list_ref x)
{
  printf("[ ");
  struct link *link = list_ref_links(x);
  while (link) {
      printf("%d ", link->value);
      link = link->next;
  }
  printf("]\n");
}


bool contains(list_ref x, int val)
{
  struct link *links = list_ref_links(x);
  while (links) {
      if (links->value == val)
          return true;
      links = links->next;
  }
  return false;
}

int prepend(list_ref x, int val)
{
  struct link *new_list =
    new_link(val, list_ref_links(x));
  if (!new_list) return 0;
  list_ref_links(x) = new_list;
  return 1;
}

// only call this with non-NULL links
struct link *last_link(struct link *x)
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
  struct link *val_link = new_link(val, 0);
  if (!val_link) return 0;

  if (is_list_ref_empty(x)) {
    list_ref_links(x) = val_link;
  } else {
    last_link(list_ref_links(x))->next = val_link;
  }
  return 1;
}

void concatenate(list_ref x, list_ref y)
{
  if (is_list_ref_empty(x)) {
    list_ref_links(x) = list_ref_links(y);
  } else {
    last_link(list_ref_links(x))->next =
      list_ref_links(y);
  }
  // remove alias to the old y links
  *y = empty_list;
}

void delete_value(list_ref x, int val)
{
  // We can use the previous implementation, but update
  // x...
}

void reverse(list_ref x)
{
  // We can use the previous implementation, but update
  // x...
}

int main(int argc, char **argv)
{
  srand(time(0));
  int array[] = { 1, 2, 3, 4, 5 };
  int n = sizeof array / sizeof *array;

  list x = make_list(n, array);
  if (!x) {
    perror("List error: ");
    exit(1); // Just bail here
  }

  printf("Contains:\n");
  printf("%d %d %d\n",
          contains(&x, 0),
          contains(&x, 3),
          contains(&x, 6));
  free_list(x);
  printf("\n");

  printf("prepend/append\n");
  x = make_list(n, array);
  if (!x) {
    perror("List error: ");
    exit(1); // Just bail here
  }

  // This is the natural way to write code...
  append(&x, 6);
  prepend(&x, 0);

  print_list(&x);
  free_list(x);
  printf("\n");


  printf("concatenate:\n");
  x = make_list(n, array);
  if (!x) {
    perror("List error: ");
    exit(1);
  }
  list y = make_list(n, array);
  if (!y) {
    perror("List error: ");
    exit(1);
  }

  concatenate(&x, &y);
  // list and list3 are the same now
  print_list(&x);
  print_list(&y);
  free_list(x);
  printf("\n");


  return EXIT_SUCCESS;
}
