#include <stdio.h>
#include <stdlib.h>

#include "refcount.h"


struct node {
  int val;
  struct node *left, *right;
};

size_t nodes;

void free_node(void *p, void *ctx)
{
  nodes--;
  struct node *n = p;
  printf("freeing node %d\n", n->val);
  decref_ctx(n->left, ctx);
  decref_ctx(n->right, ctx);
}

struct node *new_node(int val, takes struct node *left, takes struct node *right)
{
  struct node *n = rc_alloc(sizeof *n, free_node);
  if (n) {
    nodes++;
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

  printf("%zu nodes\n", nodes);

  return 0;
}
