#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "list.h"

struct list *new_list(struct list_type type)
{
  struct list *list = malloc(sizeof *list);
  if (list) {
    *list = (struct list){
      .head = { .next = &list->head,
                .prev = &list->head },
      .type = type
    };
  }
  return list;
}


void free_list(struct list *x)
{
  void (*free_link)(struct link *) = x->type.free;
  // We can only free if we have a free function.
  // Otherwise, assume that we shouldn't free.
  if (free_link) {
    struct link *link = front(x);
    while (link != head(x)) {
        struct link *next = link->next;
        free_link(link);
        link = next;
    }
  }
  free(x);
}

// Default print function
static void print_link(struct link *link)
{
  printf("<link %p>", (void *)link);
}

void print_list(struct list *x)
{
  void (*print)(struct link *) =
    (x->type.print) ? x->type.print : print_link;
  printf("[ ");
  for_each(link, x) {
    print(link);
    putchar(' ');
  }
  printf("]\n");
}

struct link *find_link(struct list *x,
                       struct link *from,
                       bool (*p)(struct link *))
{
  for_each_from(link, x, from) {
    if (p(link)) return link;
  }
  return &x->head; // head indicates the end
}

void delete_if(struct list *x, bool (*p)(struct link *))
{
  void (*free)(struct link *) = x->type.free;
  struct link *link = front(x);
  while (link != head(x)) {
    struct link *next = link->next;
    if (p(link)) {
      unlink(link);
      if (free) free(link);
    }
    link = next;
  }
}
