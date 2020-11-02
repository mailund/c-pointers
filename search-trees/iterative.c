#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

typedef struct node *stree;
struct node {
  int value;
  struct node *left;
  struct node *right;
};

stree node(int value, stree left, stree right)
{
  stree t = malloc(sizeof *t);
  if (t) *t = (struct node){
    .value = value, .left = left, .right = right
  };
  return t;
}
#define leaf(V) node(V, 0, 0)

stree *rightmost_to(stree *t, struct node *n);
#define rightmost(t) rightmost_to(t, 0)


void free_nodes(struct node *n)
{
  struct node *curr = n;
  while (curr) {
    if (!curr->left) {
      struct node *right = curr->right;
      free(curr);
      curr = right;

    } else {

      // Since we delete the left links, left will be NULL
      // when we return to node, so we never find curr again
      stree pred = *rightmost(&curr->left);
      if (pred->right == 0) {
        pred->right = curr;
        // Remove the left link so we don't go down that tree
        // again...
        struct node *left = curr->left;
        curr->left = 0;
        curr = left;

      } else {
        pred->right = 0;
        struct node *right = curr->right;
        free(curr);
        curr = right;
      }
    }
  }
}

stree *new_stree(void)
{
  stree *t = malloc(sizeof *t);
  if (t) *t = 0;
  return t;
}

#define clear_stree(t) \
  do { free_nodes(*t); *t = 0; } while(0)

#define free_stree(t) \
  do { free_nodes(*t); free(t); t = 0; } while(0)


// tail recursive
stree *find_loc(stree *t, int val)
{
  while (*t && (*t)->value != val) {
    if (val < (*t)->value)
      t = &(*t)->left;
    else
      t = &(*t)->right;
  }
  return t;
}

bool contains(stree *t, int val)
{
  return !! *find_loc(t, val);
}

bool insert(stree *t, int val)
{
  stree *target = find_loc(t, val);
  if (*target) return true; // already there
  else return !!(*target = leaf(val));
}


void delete(stree *t, int val)
{
  stree *target = find_loc(t, val);
  if (*target) {
    stree t = *target;
    if (!(t->left && t->right)) {
      *target = t->left ? t->left : t->right;
      free(t);
    } else {
      stree *rm_ref = rightmost(&t->left);
      stree rm = *rm_ref;
      t->value = rm->value;
      *rm_ref = rm->left;
      free(rm);
    }
  }
}

stree *make_stree(int n, int array[n])
{
  stree *t = new_stree();
  if (!t) return 0;
  for (int i = 0; i < n; i++) {
    if (!insert(t, array[i])) {
      free_stree(t);
      return 0;
    }
  }
  return t;
}

// Not tail recursive
void print_stree(stree *t)
{
  if (!*t) return;
  putchar('(');
    print_stree(&(*t)->left);
    putchar(',');
    printf("%d", (*t)->value);
    putchar(',');
    print_stree(&(*t)->right);
  putchar(')');
}


stree *rightmost_to(stree *t, struct node *n)
{
  while ((*t)->right && (*t)->right != n) {
    t = &(*t)->right;
  }
  return t;
}




int main(void)
{
#if 0
  stree t =
    node(3,
      node(2, leaf(1), 0),
      leaf(6));
#endif
  stree t = 0;
  insert(&t, 3);
  insert(&t, 2);
  insert(&t, 1);
  insert(&t, 6);

  print_stree(&t); putchar('\n');

  assert(contains(&t, 2));
  assert(contains(&t, 1));
  assert(contains(&t, 3));
  assert(!contains(&t, 10));
  assert(!contains(&t, 0));

  insert(&t, 10);
  insert(&t, 0);

  assert(contains(&t, 10));
  assert(contains(&t, 0));


  print_stree(&t); putchar('\n');

  delete(&t, 12);
  delete(&t, 3);
  delete(&t, 6);

  assert(!contains(&t, 3));
  assert(!contains(&t, 6));

  print_stree(&t); putchar('\n');
  clear_stree(&t);

  int array[] = { 1, 2, 13, 4, 16, 8, 10 };
  int n = sizeof array / sizeof *array;
  stree *t2 = make_stree(n, array);
  print_stree(t2); putchar('\n');

  return 0;
}
