
#include "list.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>






struct link *get_smallest(list x)
{
  assert(!is_empty(x));
  struct link *p = front(x);
  struct link *res = p;
  while (p != x) {
    if (p->value < res->value)
      res = p;
    p = p->next;
  }
  return res;
}

void move_links(list x, list y)
{
  free_links(x);
  if (!is_empty(y)) {
    *x = *y;
    connect_neighbours(x);
    clear_list(y);
  }
}

void selection_sort(list x)
{
  list_head sorted = init_list_head(sorted);
  while (!is_empty(x)) {
    struct link *next = get_smallest(x);
    unlink(next);
    append_link(&sorted, next);
  }
  move_links(x, &sorted);
}


int main(int argc, char **argv)
{
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
  list y = make_list(n, array);
  if (!y) {
    perror("List error: ");
    exit(1);
  }

  concatenate(x, y);
  print_list(x);
  free_list(x);

  // concatenate() doesn't free y, it only
  // empties it
  assert(y->prev == y);
  assert(y->next == y);
  free_list(y);

  x = make_list(n, array);
  y = new_list();
  concatenate(x, y);
  print_list(x);
  free_list(x);
  free_list(y);

  x = new_list();
  y = make_list(n, array);
  concatenate(x, y);
  print_list(x);
  free_list(x);
  free_list(y);

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

  list_head head = init_list_head(head);
  append(&head, 1);
  append(&head, 2);
  print_list(&head);
  free_links(&head);

  x = new_list();
  append(x, 4);
  append(x, 1);
  append(x, 4);
  append(x, 7);
  append(x, 3);
  selection_sort(x);
  print_list(x);
  free_list(x);

  return 0;
}
