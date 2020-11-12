#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "list.h"

// FIXME
void free_links(struct list *x)
{
  struct link *link = front(x);
  while (link != head(x)) {
      struct link *next = link->next;
      free(link);
      link = next;
  }
}

void print_list(struct list *x)
{
  printf("[ ");
  for (struct link *link = front(x);
       link != head(x);
       link = link->next) {
    // FIXME printf("%d ", link->value);
  }
  printf("]\n");
}
