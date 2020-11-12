#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "list.h"

struct int_link {
  struct link link;
  int value;
};
typedef struct int_link ilink;

struct int_link *new_int_link(int value)
{
  ilink *link = malloc(sizeof *link);
  if (link) link->value = value;
  return link;
}

void print_int_link(struct link *link)
{
  printf("%d", ((ilink *)link)->value);
}

void free_int_link(struct link *link)
{
  free(link); // Nothing special
}

struct list_type int_list = {
  .free = free_int_link,
  .print = print_int_link
};

bool is_even(struct link *l)
{
  ilink *link = (ilink *)l;
  return link->value % 2 == 0;
}

int main(void)
{
  struct list *x = new_list(int_list);
  for (int i = 0; i < 10; i++) {
    struct int_link *link = new_int_link(i);
    if (!link) abort();
    append(x, (struct link *)link);
  }
  print_list(x);

  ilink *link = (ilink *)find_link(x, front(x), is_even);
  printf("%d\n", link->value);
  link = (ilink *)find_link(x, link->link.next, is_even);
  printf("%d\n", link->value);

  for_each_iter_args(link, x, find_link, is_even) {
    printf("%d ", ((ilink *)link)->value);
  }
  printf("\n");

  delete_if(x, is_even);
  print_list(x);

  free_list(x);

  // using stack-allocated links
  ilink l1 = { .value = 13 };
  ilink l2 = { .value = 42 };
  struct list_type type = {
    .print = print_int_link,
    .free = 0 // Do not free stack allocated links
  };
  x = new_list(type);
  append(x, (struct link *)&l1);
  append(x, (struct link *)&l2);
  print_list(x);
  free_list(x);

  return 0;
}
