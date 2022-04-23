#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include <stdio.h>


struct node {
  struct node *left, *right;
  int val;
};

#define SUBPOOL_SIZE 3 // an arbitrary number...
struct subpool {
  struct subpool *next;
  struct node nodes[SUBPOOL_SIZE];
};

struct subpool *new_subpool(struct subpool *next)
{
  struct subpool *pool = malloc(sizeof *pool);
  if (pool) pool->next = next;
  return pool;
}

struct node_pool {
  size_t free_slots;
  struct subpool *subpools;
};

struct node_pool *new_pool(void)
{
  struct node_pool *pool = malloc(sizeof *pool);
  if (!pool) return 0;

  struct subpool *subpool = new_subpool(0);
  if (!subpool) { free(pool); return 0; }

  pool->free_slots = SUBPOOL_SIZE;
  return pool;
}

struct node *node_alloc(struct node_pool *pool)
{
  if (!pool->free_slots) {
    struct subpool *new_top = new_subpool(pool->subpools);
    if (!new_top) return 0;

    // Success, so add new pool to list
    pool->subpools = new_top;
    pool->free_slots = SUBPOOL_SIZE;
  }
  return &pool->subpools->nodes[--(pool->free_slots)];
}

void free_pool(struct node_pool *pool)
{
  struct subpool *sp = pool->subpools;
  while (sp) {
    struct subpool *next = sp->next;
    free(sp);
    sp = next;
  }
  free(pool);
}

int main(void)
{
  struct node_pool *pool = new_pool();
  struct node *n1 = node_alloc(pool);
  struct node *n2 = node_alloc(pool);
  struct node *n3 = node_alloc(pool);
  assert(n1 - n2 == 1);
  assert(n2 - n3 == 1);
  // Technically they could sit like that, but it is highly
  // unlikely...
  struct node *n4 = node_alloc(pool);
  assert(n3 - n4 != 1);
  struct node *n5 = node_alloc(pool);
  assert(n4 - n5 == 1);
  struct node *n6 = node_alloc(pool);
  assert(n5 - n6 == 1);

  struct node *n7 = node_alloc(pool);
  assert(n6 - n7 != 1);
  struct node *n8 = node_alloc(pool);
  assert(n7 - n8 == 1);
  struct node *n9 = node_alloc(pool);
  assert(n8 - n9 == 1);
  struct node *n10 = node_alloc(pool);
  assert(n9 - n10 != 1);

  free_pool(pool);

  return 0;
}
