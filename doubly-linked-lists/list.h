#ifndef LIST_H
#define LIST_H

#include <stdlib.h>
#include <stdbool.h>

struct link {
  int value;
  struct link *prev;
  struct link *next;
};

typedef struct link list_head;

#define init_list_head(x) \
  (list_head){ .prev = &(x), .next = &(x) }

typedef list_head *list;
#define front(x)    (x)->next
#define last(x)     (x)->prev
#define is_empty(x) ((x) == front(x))


// Doesn't free, links, but set the list to empty
#define clear_head(head) \
  do { (head) = init_list_head(head); } while(0)

// Doesn't free, links, but set the list to empty
#define clear_list(x) clear_head(*(x))

struct link *new_link(int val,
                      struct link *prev,
                      struct link *next);
list new_list(void);

void free_links(list_head *head);
#define free_list(x) \
  do { free_links(x); free(x); x = 0; } while(0)


// Connect x and y so x's next is y and y's prev is x
#define connect(x, y) \
  do { (x)->next = (y); (y)->prev = (x); } while(0)

// If x already has the right next and prev, make those
// point to x
#define connect_neighbours(x)  \
  do {                         \
    (x)->next->prev = (x);     \
    (x)->prev->next = (x);     \
  } while(0)

// Make y's prev and next point to x and x->next,
// then connect it in so its neighbours match
#define link_after(x, y)    \
  do {                      \
    (y)->prev = (x);        \
    (y)->next = (x)->next;  \
    connect_neighbours(y);  \
  } while(0)

#define link_before(x, y)   \
  link_after((x)->prev, y)

// Remove x from the list, but leave its
// pointers so we still have access to its
// neighbours if we need them
#define unlink(x)                    \
  do {                               \
    (x)->next->prev = (x)->prev;     \
    (x)->prev->next = (x)->next;     \
  } while(0)

#define delete_link(x) \
  do { unlink(x); free(x); x = 0; } while(0)

int insert_val_after(struct link *after, int val);
#define insert_val_before(before, val) \
  insert_val_after((before)->prev, val)

#define prepend_link link_after
#define append_link  link_before
#define prepend insert_val_after
#define append  insert_val_before

list make_list(int n, int array[n]);
void print_list(list x);
bool contains(list x, int val);
void concatenate(list x, list y);
void delete_value(list x, int val);
void reverse(list x);
list copy_list(list x);
bool equal(list x, list y);



#endif // LIST_H
