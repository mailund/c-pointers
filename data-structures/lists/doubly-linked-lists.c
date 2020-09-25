#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <assert.h>

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


struct link {
  int value;
  struct link *prev;
  struct link *next;
};

struct link *new_link(int val,
                      struct link *prev,
                      struct link *next)
{
  struct link *link = malloc(sizeof(struct link));
  if (!link) return 0;

  link->value = val;
  link->prev = prev;
  link->next = next;
  return link;
}

typedef struct link list;
list *new_list(void)
{
  struct link *head = new_link(0, 0, 0);
  if (!head) return 0;

  head->next = head->prev = head;
  return head;
}

void free_list(list *x)
{
  struct link *link = x->next;
  while (link != x) {
      struct link *next = link->next;
      free(link);
      link = next;
  }
  free(x);
}

int insert_val_after(struct link *after, int val)
{
  struct link *link = new_link(val, after, after->next);
  if (!link) return 0;
  link->prev->next = link->next->prev = link;
  return 1;
}

int prepend(struct link *head, int val)
{
  return insert_val_after(head, val);
}

int append(struct link *head, int val)
{
  return insert_val_after(head->prev, val);
}

list *make_list(int n, int array[n])
{
  struct link *x = new_list();
  if (!x) return 0;

  // From this point on, x is in a
  // consistent state so we can free it
  // if something goes wrong
  for (int i = 0; i < n; i++) {
    if (!append(x, array[i])) {
      free_list(x);
      return 0;
    }
  }
  return x;
}

void print_list(list *x)
{
  printf("[ ");
  struct link *link = x->next;
  while (link != x) {
    printf("%d ", link->value);
    link = link->next;
  }
  printf("]\n");
}


bool contains(list *x, int val)
{
  struct link *link = x->next; // pass head
  while (link != x) {
    if (link->value == val)
      return true;
    link = link->next;
  }
  return false;
}


// We don't delete y, but we empty it.
// The caller must free it if he no longer
// needs it. We could free it here, that
// just changes the API. It is a design
// choice.
void concatenate(struct link *x, struct link *y)
{
  // If y is empty we are already done
  if (y->next == y) return;

  struct link *x_last = x->prev;
  struct link *y_first = y->next;
  struct link *y_last = y->prev;

  x_last->next = y_first; y_first->prev = x_last;
  y_last->next = x; x->prev = y_last;

  // remove elements from y
  y->prev = y->next = y;
}

void remove_link(struct link *link)
{
  struct link *prev = link->prev;
  struct link *next = link->next;
  prev->next = next; next->prev = prev;
  free(link);
}

void delete_value(list *x, int val)
{
  struct link *link = x->next;
  while (link != x) {
    struct link *next = link->next;
    if (link->value == val)
      remove_link(link);
    link = next;
  }
}

void prepend_link(list *x, struct link *link)
{
  link->prev = x; link->next = x->next;
  link->prev->next = link->next->prev = link;
}

void reverse(list *x)
{
  if (x->next == x) return; // Empty list

  // stack allocated so we don't need to handle
  // malloc errors...
  list y; y.prev = y.next = &y;

  // link points to an actual link, not the head
  struct link *link = x->next;
  // next might point to x, in which case we should
  // stop, but only after appending link.
  struct link *next = link->next;
  // Append link, it is not x, and move it
  // to the next link
  do {
    assert(link != x);
    link->prev = link->next = 0;
    prepend_link(&y, link);
    link = next;
    next = link->next;
  } while (link != x);

  // Move the elements back to x
  x->prev = y.prev; x->prev->next = x;
  x->next = y.next; x->next->prev = x;
}

int main(int argc, char **argv)
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
  // concatenate() doesn't free y, it only
  // empties it
  assert(y->prev == y);
  assert(y->next == y);
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

  return 0;
}
