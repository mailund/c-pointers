#include <stdio.h>
#include <assert.h>
#include "list.h"
#include "dynarray.h"

typedef void *   stack;
typedef int       elem;

typedef struct {
  stack (*new_stack)    (void);
  bool  (*empty_stack)  (stack);
  elem  (*top)          (stack);
  bool  (*push)         (stack, elem);
  elem  (*pop)          (stack);
  void  (*free_stack)   (stack);
} stack_type;

stack new_stack(stack_type *stype)
{
  return stype->new_stack();
}
bool empty_stack(stack_type *stype, stack stack)
{
  return stype->empty_stack(stack);
}
elem top(stack_type *stype, stack stack)
{
  return stype->top(stack);
}
bool push(stack_type *stype, stack stack, elem elem)
{
  return stype->push(stack, elem);
}
elem pop(stack_type *stype, stack stack)
{
  return stype->pop(stack);
}
void free_stack(stack_type *stype, stack stack)
{
  stype->free_stack(stack);
}

// List stack
stack list_stack_new(void)
{
  return new_list();
}
bool list_stack_empty(stack stack)
{
  list x = stack;
  return front(x) == x;
}
elem list_stack_top(stack stack)
{
  list x = stack;
  return front(x)->value;
}
bool list_stack_push(stack stack, elem elem)
{
  return prepend(stack, elem);
}
elem list_stack_pop(stack stack)
{
  list x = stack;
  struct link *top = front(x);
  elem elem = top->value;
  delete_link(top);
  return elem;
}
void list_stack_free(stack stack)
{
  list x = stack;
  free_list(x);
}


int main(void)
{
  stack stack = list_stack_new();
  list_stack_push(stack, 1);
  list_stack_push(stack, 2);
  while (!list_stack_empty(stack)) {
    int x = list_stack_pop(stack);
    printf("%d\n", x);
  }
  list_stack_free(stack);

  return 0;
}
