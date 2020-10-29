#include "list.h"
#include <assert.h>
#include <time.h>
#include <stdio.h>

bool is_sorted(list x)
{
  struct link *p = front(x);
  while (p->next != x) {
    if (p->value > p->next->value)
      return false;
    p = p->next;
  }
  return true;
}

// Selection sort ------------------------------
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


// Insertion sort -----------------------------------------------
#if 0
// Insert from the beginning. Not the usual insertion sort. Not stable
void insert_sorted(list x, struct link *link)
{
  struct link *p = front(x);
  while (p != x && p->value < link->value)
    p = p->next;
  link_before(p, link);
}

#else
// Insert from the end; this is stable
void insert_sorted(list x, struct link *link)
{
  struct link *p = last(x);
  while (p != x && p->value > link->value)
    p = p->prev;
  link_after(p, link);
}
#endif

#if 0
void insertion_sort(list x)
{
  list_head sorted = init_list_head(sorted);
  struct link *p = front(x);
  while (p != x) {
    struct link *next = p->next;
    unlink(p);
    insert_sorted(&sorted, p);
    p = next;
  }
  move_links(x, &sorted);
}
#else
void insertion_sort(list x)
{
  list_head sorted = init_list_head(sorted);
  while (!is_empty(x)) {
    struct link *p = front(x);
    unlink(p);
    insert_sorted(&sorted, p);
  }
  move_links(x, &sorted);
}
#endif

// Merge sort -----------------------------------------------

// Quick sort -----------------------------------------------


list random_list(int n)
{
  list x = new_list();
  if (!x) abort();

  for (int i = 0; i < n; i++) {
    if (!append(x, rand() % 10)) abort();
  }

  return x;
}

void test_sorting(int n)
{
  list x = random_list(n);
  printf("x:         "); print_list(x);

  list y = copy_list(x);
  selection_sort(y);
  printf("selection: "); print_list(y);
  assert(is_sorted(y));
  free_list(y);

  y = copy_list(x);
  insertion_sort(y);
  printf("insertion: "); print_list(y);
  assert(is_sorted(y));
  free_list(y);

  printf("\n");
  free_list(x);
}

int main(void)
{
  int seed = time(NULL);
  srand(seed);

  for (int i = 0; i < 5; i++) {
    test_sorting(10);
  }

  return 0;
}
