#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <stddef.h>   // for max_align_t
#include <stdalign.h> // for alignof
#define MAXALIGN alignof(max_align_t)

struct refcount {
  size_t rc;
  void (*cleanup)(void *, void *);
  void *stack;
};

// On my machine, I need two MAXALIGN to fit the refcount structure.
// You will have to check on your machine as well...
#define REFCOUNT_MEM (MAXALIGN << 1)

#define refcount_mem(p) \
  (struct refcount *)((char *)p - REFCOUNT_MEM)
#define user_mem(rc) \
  (void *)((char *)rc + REFCOUNT_MEM)

void *rc_alloc(size_t size, void (*cleanup)(void *, void *))
{
  static_assert(REFCOUNT_MEM >= sizeof(struct refcount),
                "The refcount struct won't fit in the allocated memory!");

  struct refcount *rc = malloc(REFCOUNT_MEM + size);
  if (!rc) return 0;

  rc->rc = 1;
  rc->cleanup = cleanup;
  rc->stack = 0; // FIXME

  return user_mem(rc);
}

void *incref(void *p);
void *decref(void *p);


void *incref(void *p)
{
  if (!p) return p; // it makes some code easier if we allow this
  struct refcount *rc = refcount_mem(p);
  rc->rc++;
  return p;
}

void cleanup(struct refcount *stack)
{
  printf("cleanup!\n");
  while (stack) {
    if (stack->cleanup)
      stack->cleanup(user_mem(stack), stack);
    struct refcount *next = stack->stack;
    free(stack);
    stack = next;
  }
}

void *decref_ctx(void *p, void *ctx)
{
  if (!p) return p; // accept NULL as free() would...

  struct refcount *rc = refcount_mem(p);
  if (--rc->rc == 0) {
    if (ctx) {
      // Schedule for deletion
      printf("schedule for deletion...\n");
      struct refcount *stack = ctx;
      rc->stack = stack->stack;
      stack->stack = rc;
    } else {
      // Start cleanup
      cleanup(rc);
    }
    return 0; // reference is now gone...
  }

  return p;
}

#define decref(p) decref_ctx(p, 0)


#define borrows
#define takes
#define give(x) x

struct node {
  int val;
  struct node *left, *right;
};

void free_node(void *p, void *ctx)
{
  struct node *n = p;
  printf("freeing node %d\n", n->val);
  decref_ctx(n->left, ctx);
  decref_ctx(n->right, ctx);
}

struct node *new_node(int val, takes struct node *left, takes struct node *right)
{
  struct node *n = rc_alloc(sizeof *n, free_node);
  if (n) {
    n->val = val;
    n->left = give(left);
    n->right = give(right);
  }
  return n;
}

void print_tree(struct node *n)
{
  if (!n) return;
  putchar('(');
    print_tree(n->left);
    printf(",%d,", n->val);
    print_tree(n->right);
  putchar(')');
}

int main(void)
{
  struct node *x = new_node(1, new_node(0, 0, 0),
                               new_node(3, new_node(2, 0, 0), 0));
  print_tree(x); putchar('\n');

  struct node *y = new_node(10, new_node(9, 0, 0), new_node(11, 0, 0));
  struct node *z = new_node(5, incref(x), incref(y));
  print_tree(z); putchar('\n');

  decref(x);
  print_tree(z); putchar('\n'); // z is still here...
  decref(z);
  print_tree(y); putchar('\n'); // y is still here...

  // free the rest
  decref(y);

  return 0;
}
