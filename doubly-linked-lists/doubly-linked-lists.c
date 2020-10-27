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
  struct link *link = malloc(sizeof *link);
  if (!link) return 0;

  link->value = val;
  link->prev = prev;
  link->next = next;
  return link;
}




typedef struct link list_head;

#define init_list_head(x) \
  (list_head){ .prev = &(x), .next = &(x) }

typedef list_head *list;
#define front(x)    (x)->next
#define last(x)     (x)->prev
#define is_empty(x) ((x) == front(x))


list new_list(void)
{
  struct link *head = new_link(0, 0, 0);
  if (!head) return 0;
  *head = init_list_head(*head);
  return head;
}

void free_links(list_head *head)
{
  struct link *link = front(head);
  while (link != head) {
      struct link *next = link->next;
      free(link);
      link = next;
  }
}

// Doesn't free, links, but set the list to empty
#define clear_head(head) \
  do { (head) = init_list_head(head); } while(0)

void free_list(list x)
{
  free_links(x);
  free(x);
}

// Doesn't free, links, but set the list to empty
#define clear_list(x) clear_head(*(x))


// Connect x and y so x's next is y and y's prev is x
static inline
void connect(struct link *x, struct link *y)
{
  x->next = y;
  y->prev = x;
}

// If x already has the right next and prev, make those
// point to x
static inline
void connect_neighbours(struct link *x)
{
  x->next->prev = x;
  x->prev->next = x;
}

// Make y's prev and next point to x and x->next,
// then connect it in so its neighbours match
static inline
void link_after(struct link *x,
                struct link *y)
{
  y->prev = x; y->next = x->next;
  connect_neighbours(y);
}

static inline
void link_before(struct link *x,
                 struct link *y)
{
  link_after(x->prev, y);
}

static inline
void prepend_link(list x, struct link *link)
{
  link_after(x, link);
}

static inline
void append_link(list x, struct link *link)
{
  link_before(x, link);
}

// Remove x from the list, but leave its
// pointers so we still have access to its
// neighbours if we need them
static inline
void unlink(struct link *x)
{
  x->next->prev = x->prev;
  x->prev->next = x->next;
}

void delete_link(struct link *link)
{
  unlink(link);
  free(link);
}

int insert_val_after(struct link *after, int val)
{
  struct link *link =
    new_link(val, after, after->next);
  if (!link) return 0;
  connect_neighbours(link);
  return 1;
}

static inline
int insert_val_before(struct link *before, int val)
{
  return insert_val_after(before->prev, val);
}

static inline
int prepend(list x, int val)
{
  return insert_val_after(x, val);
}

static inline
int append(list x, int val)
{
  return insert_val_before(x, val);
}

list make_list(int n, int array[n])
{
  list x = new_list();
  if (!x) return 0;

  // going in the forward direction and appending
  for (int i = 0; i < n; i++) {
    if (!append(x, array[i])) {
      free_list(x);
      return 0;
    }
  }

  return x;
}

void print_list(list x)
{
  printf("[ ");
  struct link *link = front(x);
  while (link != x) {
    printf("%d ", link->value);
    link = link->next;
  }
  printf("]\n");
}


bool contains(list x, int val)
{
  struct link *link = front(x);
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
void concatenate(list x, list y)
{
  // If y is empty we are already done
  if (is_empty(y)) return;

  connect(last(x), front(y));
  connect(last(y), x);

  // remove elements from y
  clear_list(y);
}

void delete_value(list x, int val)
{
  struct link *link = front(x);
  while (link != x) {
    struct link *next = link->next;
    if (link->value == val)
      delete_link(link);
    link = next;
  }
}

#if 0
#define swap_int(x,y) \
  do { int tmp = x; x = y; y = tmp; } while(0)

void reverse(list x)
{
  struct link *left = front(x);
  struct link *right = last(x);
  while (left != right) {
    swap_int(left->value, right->value);
    left = left->next; right = right->prev;
  }
}

#else

#define swap_p(x,y) \
  do { struct link *tmp = x; x = y; y = tmp; } while(0)

void reverse(list x)
{
  struct link *p = x;
  do {
    swap_p(p->prev, p->next);
    p = p->prev;
  } while (p != x);
}

#endif

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
  if (is_empty(y)) return;
  connect(x, front(y));
  connect(last(y), x);
  clear_list(y);
}

void selection_sort(list x)
{
  list_head y = init_list_head(y);
  while (!is_empty(x)) {
    struct link *next = get_smallest(x);
    unlink(next);
    append_link(&y, next);
  }
  move_links(x, &y);
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
