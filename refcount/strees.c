#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include "refcount.h"


struct node {
  int const val;
  struct node * const left;
  struct node * const right;
};


struct node *get_EMPTY(void)
{
  static struct node *empty_node = 0;
  if (!empty_node) empty_node = rc_alloc(sizeof *empty_node, 0);
  if (!empty_node) abort(); // nothing works without it
  return empty_node;
}

#define EMPTY incref(get_EMPTY())

static inline 
bool is_empty(borrows struct node *t)
{ return t == get_EMPTY(); }
static inline
bool is_error(borrows struct node *t)
{ return t == 0; }

size_t nodes;

void free_node(void *p, void *ctx)
{
  nodes--;
  struct node *n = p;
  printf("freeing node %d\n", n->val);
  decref_ctx(n->left, ctx);
  decref_ctx(n->right, ctx);
}

struct node *new_node(int val,
                      takes struct node *left,
                      takes struct node *right)
{
  if (is_error(left) || is_error(right)) {
    decref(left); decref(right); return 0;
  }

  struct node *n = rc_alloc(sizeof *n, free_node);
  if (!n) goto error;

  nodes++; // FIXME: debug

  memcpy(n,
    &(struct node) {
      .val = val, .left = transfer(left), .right = transfer(right)
    },
    sizeof *n);
  return n;

error:
  decref(left); decref(right);
  return 0;
}

bool contains(borrows struct node *tree, int val)
{
  assert(!is_error(tree));
  if (is_empty(tree))   return false;
  if (tree->val == val) return true;
  if (val < tree->val)  return contains(tree->left, val);
  else                  return contains(tree->right, val);
}

struct node *insert(takes struct node *tree, int val)
{
  if (is_error(tree)) return 0;
  if (is_empty(tree)) {
    decref(tree);
    return new_node(val, EMPTY, EMPTY);
  }
  if (val == tree->val) return transfer(tree);

  int tval = tree->val;
  struct node *left = incref(tree->left);
  struct node *right = incref(tree->right);
  decref(tree);

  if (val < tree->val) {
    return new_node(tree->val,
                    insert(transfer(left), val),
                    transfer(right));
  } else {
    return new_node(tree->val,
                    transfer(left),
                    insert(transfer(right), val));
  }
}

int rightmost_value(borrows struct node *tree)
{
  assert(!is_empty(tree));
  while (!is_empty(tree->right)) tree = tree->right;
  return tree->val;
}

struct node *delete(takes struct node *tree, int val)
{
  if (is_empty(tree)) return transfer(tree);

  int tval = tree->val;
  struct node *left = incref(tree->left);
  struct node *right = incref(tree->right);
  decref(tree);

  if (val < tval) {
    return new_node(tval, delete(transfer(left), val), transfer(right));
  } else if (val > tval) {
    return new_node(tval, transfer(left), delete(transfer(right), val));
  } else {
    if (is_empty(left))  { decref(left);  return transfer(right); }
    if (is_empty(right)) { decref(right); return transfer(left);  }
    int rmval = rightmost_value(left);
    return new_node(rmval, delete(transfer(left), rmval), transfer(right));
  }
}

void print_tree(borrows struct node *n)
{
  if (is_empty(n)) return;
  putchar('(');
    print_tree(n->left);
    printf(",%d,", n->val);
    print_tree(n->right);
  putchar(')');
}

int main(void)
{
  struct node *x = new_node(1, new_node(0, EMPTY, EMPTY),
                               new_node(3, new_node(2, EMPTY, EMPTY), EMPTY));
  print_tree(x); putchar('\n');

  struct node *y = new_node(10, new_node(9, EMPTY, EMPTY), new_node(11, EMPTY, EMPTY));
  struct node *z = new_node(5, incref(x), incref(y));
  print_tree(z); putchar('\n');

  decref(x);
  print_tree(z); putchar('\n'); // z is still here...
  decref(z);
  print_tree(y); putchar('\n'); // y is still here...

  // free the rest
  decref(y);

  printf("%zu nodes\n", nodes);

  x = insert(EMPTY, 5);
  print_tree(x); putchar('\n');
  x = insert(x, 2);
  print_tree(x); putchar('\n');
  x = insert(x, 4);
  print_tree(x); putchar('\n');
  x = insert(x, 8);
  print_tree(x); putchar('\n');

  printf("%zu nodes\n", nodes);
  decref(x);
  printf("%zu nodes\n", nodes);

  printf("rebuilding x\n");
  x = insert(EMPTY, 5);
  print_tree(x); putchar('\n');
  x = insert(x, 2);
  print_tree(x); putchar('\n');
  x = insert(x, 4);
  print_tree(x); putchar('\n');
  x = insert(x, 8);
  print_tree(x); putchar('\n');

  printf("deleting from x\n");
  x = delete(x, 1);
  print_tree(x); putchar('\n');
  x = delete(x, 2);
  print_tree(x); putchar('\n');
  x = delete(x, 4);
  print_tree(x); putchar('\n');

  printf("building y\n");
  y = insert(incref(x), 1); // incref x here to keep it!
  print_tree(x); putchar('\t'); print_tree(y); putchar('\n');
  y = insert(y, 2);
  print_tree(x); putchar('\t'); print_tree(y); putchar('\n');
  y = insert(y, 3);
  print_tree(x); putchar('\t'); print_tree(y); putchar('\n');

  printf("deleting 5 (it is in both lists)\n");
  x = delete(x, 5); printf("didn't trigger deletion...\n");
  y = delete(y, 5); printf("but this did!\n");
  print_tree(x); putchar('\t'); print_tree(y); putchar('\n');

  printf("cleaning up...\n");
  printf("%zu nodes\n", nodes);
  decref(x);
  printf("%zu nodes\n", nodes);
  decref(y);
  printf("%zu nodes\n", nodes);


  printf("chapter example...\n");
  x = insert(EMPTY, 5);
  x = insert(x, 1);
  x = insert(x, 2);
  x = insert(x, 3);

  y = insert(incref(x), 10);
  y = insert(y, 9);

  printf("do I delete now?\n");
  x = delete(x, 2);

  return 0;
}
