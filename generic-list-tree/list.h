#ifndef LIST_H
#define LIST_H

#include <stdlib.h>
#include <stdbool.h>

struct link {
  struct link *prev;
  struct link *next;
};

static inline
void connect(struct link *x, struct link *y)
{ x->next = y; y->prev = x; }

static inline
void connect_neighbours(struct link *x)
{ x->next->prev = x; x->prev->next = x; }

static inline
void link_after(struct link *x, struct link *y)
{
  y->prev = x; y->next = x->next;
  connect_neighbours(y);
}
#define link_before(x, y) link_after((x)->prev, y)

static inline
void unlink(struct link *x)
{
  x->next->prev = x->prev;
  x->prev->next = x->next;
}

struct list {
  struct link head;

};

#define head(x)     (&(x)->head)
#define front(x)    (head(x)->next)
#define back(x)     (head(x)->prev)
#define is_empty(x) (head(x) == front(x))

#define append(x,link)   link_before(head(x), link)
#define prepend(x, link) link_after(head(x), link)


void print_list(struct list *x);


#endif // LIST_H
