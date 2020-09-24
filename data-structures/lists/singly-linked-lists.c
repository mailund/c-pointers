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
    if (!link) return 0;
    link->value = val;
    link->next = next;
    return link;
}

void free_list(struct link *list)
{
    while (list) {
        // Remember next, we cannot get it
        // after free(list)
        struct link *next = list->next;
        free(list);
        list = next;
    }
}

struct link *make_list(int n, int array[n])
{
    struct link *list = 0;
    for (int i = n - 1; i >= 0; i--) {
      struct link *link = new_link(array[i], list);
      if (!link) {
        free_list(list);
        return 0;
      }
      list = link;
    }
    return list;
}

void print_list(struct link *list)
{
    printf("[ ");
    while (list) {
        printf("%d ", list->value);
        list = list->next;
    }
    printf("]\n");
}


bool contains(struct link *list, int val)
{
    while (list) {
        if (list->value == val)
            return true;
        list = list->next;
    }
    return false;
}

struct link *prepend(struct link *list, int val)
{
    return new_link(val, list);
}

struct link *append(struct link *list, int val)
{
    if (!list) return new_link(val, 0);

    struct link *last = list;
    while (last->next) {
        last = last->next;
    }
    last->next = new_link(val, 0);
    return list;
}

struct link *concatenate(struct link *x, struct link *y)
{
    if (!x) return y;
    struct link *last = x;
    while (last->next) {
        last = last->next;
    }
    last->next = y;
    return x;
}

struct link *append_(struct link *list, int val)
{
    return concatenate(list, new_link(val, 0));
}

struct link *delete_value(struct link *list, int val)
{
    if (!list) return 0;
    if (list->value == val) {
        struct link *next = list->next;
        free(list);
        return delete_value(next, val);
    } else {
        list->next = delete_value(list->next, val);
        return list;
    }
}

struct link *reverse(struct link *list)
{
    if (!list) return 0;
    struct link *next = list->next;
    struct link *reversed = list; reversed->next = 0;
    while (next) {
        struct link *next_next = next->next;
        next->next = reversed;
        reversed = next;
        next = next_next;
    }
    return reversed;
}

int main(int argc, char **argv)
{
  int array[] = { 1, 2, 3, 4, 5 };
  int n = sizeof(array) / sizeof(int);
  struct link *list = make_list(n, array);
  print_list(list);
  free_list(list);

  list = make_list(n, array);
  if (!list) exit(1); // Just bail here

  printf("%d %d %d\n",
          contains(list, 0),
          contains(list, 3),
          contains(list, 6));
  free_list(list);
  printf("\n");

  printf("prepend/append\n");
  list = make_list(n, array);
  if (!list) exit(1); // Just bail here

  // This is the natural way to write code...
  list = append(list, 6);
  list = prepend(list, 0);

  // The error handling forces us to keep the old
  // list around
  struct link *new_list = append(list, 6);
  if (!new_list) exit(1);
  list = new_list;
  new_list = prepend(list, 0);
  if (!new_list) exit(1);
  list = new_list;

  print_list(list);
  free_list(list);
  printf("\n");

  printf("memory issues...\n");
  list = make_list(n, array);
  new_list = prepend(list, -1);
  if (!new_list) exit(1);
  struct link *list2 = new_list;

  print_list(list);
  print_list(list2);

  free_list(list);
  // WARNING WARNING WARNING!
  // Now, list2->next points to garbage!
  free(list2); // but we can delete just the link...
  printf("\n");

  return 0;

  list = make_list(n, array);
  list2 = make_list(n, array);
  struct link *list3 = concatenate(list, list2);
  // list and list3 are the same now
  print_list(list);
  print_list(list3);
  free_list(list3); // this also deletes list and list2!
  // If we had deleted list2, we couldn't delete list/list2 now

  list = make_list(n, array);
  list = append_(list, 6);
  print_list(list);
  free_list(list);

  list = make_list(n, array);
  list = delete_value(list, 2);
  list = delete_value(list, 3);
  print_list(list);
  free_list(list);

  list2 = delete_value(list, 1);
  // here, we deleted the link that list points to!
  // Therefore, we cannot call free_list(list);
  free_list(list2);

  list = make_list(n, array);
  list = reverse(list);
  print_list(list);
  free_list(list);

  return EXIT_SUCCESS;
}
