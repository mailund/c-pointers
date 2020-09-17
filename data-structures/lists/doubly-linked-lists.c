#include <stdlib.h>
#include <stdio.h>

// for contains
#include <stdbool.h>

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
    link->value = val;
    link->prev = prev;
    link->next = next;
    return link;
}

struct link *new_list(void)
{
    struct link *head = new_link(0, 0, 0);
    head->next = head->prev = head;
    return head;
}

void insert_after(struct link *link, int val)
{
    struct link *n = new_link(val, link, link->next);
    n->prev->next = n->next->prev = n;
}

void prepend(struct link *head, int val)
{
    insert_after(head, val);
}

void append(struct link *head, int val)
{
    insert_after(head->prev, val);
}

struct link *make_list(int n, int array[n])
{
    struct link *head = new_list();
    for (int i = 0; i < n; i++) {
        append(head, array[i]);
    }
    return head;
}

void print_list(struct link *head)
{
    printf("[ ");
    struct link *link = head->next;
    while (link != head) {
        printf("%d ", link->value);
        link = link->next;
    }
    printf("]\n");
}

void free_list(struct link *head)
{
    struct link *link = head->next;
    while (link != head) {
        struct link *next = link->next;
        free(link);
        link = next;
    }
}

bool contains(struct link *head, int val)
{
    struct link *link = head->next; // pass head
    while (link != head) {
        if (link->value == val)
            return true;
        link = link->next;
    }
    return false;
}


// FIXME: test if x is empty
void concatenate(struct link *x, struct link *y)
{
    if (y->next == y) // y is empty
        return;

    struct link *xlast = x->prev;
    struct link *yfirst = y->next;
    struct link *ylast = y->prev;

    xlast->next = yfirst; yfirst->prev = xlast;
    ylast->next = x; x->prev = ylast;

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

void delete_value(struct link *head, int val)
{
    struct link *link = head->next;
    while (link != head) {
        struct link *next = link->next;
        if (link->value == val) 
            remove_link(link);
        link = next;
    }
}

void reverse(struct link *head)
{
#warning not implemented
    if (!head->next) return;
    struct link *next = head->next->next;
    struct link *reversed = head->next; 
    reversed->next = 0;
    while (next) {
        struct link *next_next = next->next;
        next->next = reversed;
        reversed = next;
        next = next_next;
    }
    head->next = reversed;
}

int main(int argc, char **argv)
{
  int array[] = { 1, 2, 3, 4, 5 };
  int n = sizeof(array) / sizeof(int);
  struct link *list = make_list(n, array);
  print_list(list);
  free_list(list);

  list = make_list(n, array);
  printf("%d %d %d\n",
          contains(list, 0),
          contains(list, 3),
          contains(list, 6));
  free_list(list);
  
  list = make_list(n, array);
  append(list, 6);
  prepend(list, 0);
  print_list(list);
  free_list(list);
  
  list = make_list(n, array);
  struct link *list2 = make_list(n, array);
  concatenate(list, list2);
  print_list(list);
  free_list(list);
  free_list(list2); // safe
 

  list = make_list(n, array);
  delete_value(list, 2);
  delete_value(list, 3);
  print_list(list);
  free_list(list);
  
#if 0
  list = make_list(n, array);
  reverse(list);
  print_list(list);
  free_list(list);

  list = new_link(0, 0);
  reverse(list);
  print_list(list);
  free_list(list);
#endif

  return EXIT_SUCCESS;
}