#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// fake malloc
void * (*ml)(size_t) = malloc;
void *fake_malloc(size_t size)
{
  if (rand() % 1000 == 0) {
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
typedef struct link list;

struct link *new_link(int val, struct link *next)
{
  struct link *link = malloc(sizeof(struct link));
  if (!link) return 0;
  link->value = val;
  link->next = next;
  return link;
}

list *new_list(void)
{
  // It doesn't matter what val is
  // we are not supposed to look at it.
  return new_link(0, 0);
}

void free_links(struct link *link)
{
  while (link) {
      // Remember next, we cannot get it
      // after free(list)
      struct link *next = link->next;
      free(link);
      link = next;
  }
}

void free_list(list *x)
{
  free_links(x->next);
  x->next = 0;
}

list *make_list(int n, int array[n])
{
  list *x = new_list();
  if (!x) return 0;

  struct link *links = 0;
  for (int i = n - 1; i >= 0; i--) {
    struct link *link = new_link(array[i], links);
    if (!link) { // Allocation error -- clean up
      free_list(links);
      return 0;
    }
    links = link;
  }

  x->next = links;
  return x;
}

void print_list(list *x)
{
  printf("[ ");
  struct link *link = x->next;
  while (link) {
    printf("%d ", link->value);
    link = link->next;
  }
  printf("]\n");
}


bool contains(list *x, int val)
{
  struct link *link = x->next;
  while (link) {
    if (link->value == val)
      return true;
    link = link->next;
  }
  return false;
}

int prepend(list *x, int val)
{
  struct link *link = new_link(val, x->next);
  if (!link) return 0;
  x->next = link;
  return 1;
}

struct link *last_link(list *x)
{
  struct link *prev = x;
  while (prev->next) {
    prev = prev->next;
  }
  return prev;
}

int append(list *x, int val)
{
  struct link *link = new_link(val, 0);
  if (!link) return 0;
  struct link *last = last_link(x);
  last->next = link;
  return 1;
}

void concatenate(list *x, list *y)
{
  struct link *last = last_link(x);
  last->next = y->next;
  y->next = 0;
}

void delete_value(list *x, int val)
{
  struct link *prev = x;
  struct link *link = x->next;
  while (link) {
    if (link->value != val) {
      prev = link;
      link = link->next;
    } else {
      struct link *next = link->next;
      free(link);
      prev->next = link = next;
    }
  }
}

void reverse(list *x)
{
  if (!x->next) return;

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

  struct link *x = make_list(n, array);
  if (!x) {
    perror("List error: ");
    exit(1); // Just bail here
  }

  printf("Contains:\n");
  printf("%d %d %d\n",
          contains(x, 0),
          contains(x, 3),
          contains(x, 6));
  free_list(x);
  printf("\n");

  printf("prepend/append\n");
  x = make_list(n, array);
  if (!x) {
    perror("List error: ");
    exit(1); // Just bail here
  }

  int success = append(x, 6);
  if (!success) {
    perror("List error:");
    exit(1);
  }
  success = prepend(x, 0);
  if (!success) {
    perror("List error:");
    exit(1);
  }
  print_list(x);
  free_list(x);
  printf("\n");




  printf("concatenate:\n");
  x = make_list(n, array);
  if (!x) {
    perror("List error: ");
    exit(1);
  }
  list *y = make_list(n, array);
  if (!y) {
    perror("List error: ");
    exit(1);
  }

  concatenate(x, y);
  print_list(x);
  free_list(x);
  printf("\n");

  printf("deleting values:\n");
  x = make_list(n, array);
  if (!x) {
    perror("List error: ");
    exit(1);
  }
  delete_value(x, 2);
  delete_value(x, 3);
  print_list(x);
  free_list(x);
  printf("\n");

  printf("deleting first link:\n");
  x = make_list(n, array);
  delete_value(x, 1);
  print_list(x);
  free_list(x);
  printf("\n");

  printf("reversing:\n");
  x = make_list(n, array);
  reverse(x);
  print_list(x);
  free_list(x);
  printf("\n");

  return 0;
}
