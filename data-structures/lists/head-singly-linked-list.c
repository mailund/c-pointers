#include <stdlib.h>
#include <stdio.h>

// for contains
#include <stdbool.h>

struct link {
  int value;
  struct link *next;
};

struct link *new_link(int val, struct link *next)
{
    struct link *link = malloc(sizeof(struct link));
    link->value = val;
    link->next = next;
    return link;
}

struct link *make_list(int n, int array[n])
{
    struct link *link = 0;
    for (int i = n - 1; i >= 0; i--) {
        link = new_link(array[i], link);
    }
    struct link *head = new_link(0, link);
    return head;
}

void print_list(struct link *list)
{
    printf("[ ");
    struct link *link = list->next;
    while (link) {
        printf("%d ", link->value);
        link = link->next;
    }
    printf("]\n");
}

void free_list(struct link *list)
{
    // Nothing to change here, we also need to free
    // the head
    while (list) {
        // Remember next, we cannot get it
        // after free(list)
        struct link *next = list->next;
        free(list);
        list = next;
    }
}

bool contains(struct link *list, int val)
{
    list = list->next; // pass head
    while (list) {
        if (list->value == val)
            return true;
        list = list->next;
    }
    return false;
}

void prepend(struct link *list, int val)
{
    list->next = new_link(val, list->next);
}

void append(struct link *list, int val)
{
    while (list->next) {
        list = list->next;
    }
}

void concatenate(struct link *x, struct link *y)
{
    while (x->next) {
        x = x->next;
    }
    x->next = y->next; // skip head
    y->next = 0; // make y empty to avoid problems
}

void append_(struct link *list, int val)
{
    // append via concatenate is no longer attractive
    struct link *y = new_link(0, new_link(val, 0));
    concatenate(list, y);
    free(y);
}

void delete_value(struct link *list, int val)
{
    struct link *prev = list;
    struct link *link = list->next;
    while (link) {
        if (link->value != val) {
            prev = link;
            link = link->next;
        } else {
            struct link *next = link->next;
            free(link);
            prev->next = link = next;
        }
    }
}

void reverse(struct link *head)
{
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
  append_(list, 6);
  print_list(list);
  free_list(list);
  
  list = make_list(n, array);
  delete_value(list, 2);
  delete_value(list, 3);
  print_list(list);
  free_list(list);
  
  list = make_list(n, array);
  reverse(list);
  print_list(list);
  free_list(list);

  list = new_link(0, 0);
  reverse(list);
  print_list(list);
  free_list(list);

  return EXIT_SUCCESS;
}