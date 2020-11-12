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

// This time, unlink will set x's pointers to NULL.
// We don't want to risk the callback function modifying
// the list after the link is removed.
static inline
void unlink(struct link *x)
{
  if (!x->prev || !x->next) return;
  x->next->prev = x->prev;
  x->prev->next = x->next;
  x->prev = x->next = 0;
}

struct list_type {
  void  (*free)(struct link *);
  void (*print)(struct link *);
};
struct list {
  struct link head;
  struct list_type type;
};

#define head(x)     (&(x)->head)
#define front(x)    (head(x)->next)
#define back(x)     (head(x)->prev)
#define is_empty(x) (head(x) == front(x))

#define append(x,link)   link_before(head(x), link)
#define prepend(x, link) link_after(head(x), link)

// --Iteration macros-----------------------------
#define for_each_from(lnk, x, from)      \
  for (struct link *lnk = from;          \
       lnk != head(x); lnk = lnk->next)

#define for_each(lnk, x) \
  for_each_from(lnk, x, front(x))

#define for_each_iter(lnk, x, iter)          \
  for (struct link *lnk = iter(x, front(x)); \
       lnk != head(x);                       \
       lnk = iter(x, lnk->next))

#define for_each_iter_args(lnk, x, iter, ...)             \
  for (struct link *lnk = iter(x, front(x), __VA_ARGS__); \
       lnk != head(x);                                    \
       lnk = iter(x, lnk->next, __VA_ARGS__))

struct list *new_list(struct list_type type);

void  free_list(struct list *x);
void print_list(struct list *x);

struct link *find_link(struct list *x,
                       struct link *from,
                       bool (*p)(struct link *));
void         delete_if(struct list *x,
                       bool (*p)(struct link *));


#endif // LIST_H
