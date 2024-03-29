#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include <stdio.h>


struct node {
  struct node *left, *right;
  int val;
};

#define SUBPOOL_SIZE 3 // an arbitrary number...


union node_free_list {
  union node_free_list *next_free;
  struct node node;
};
struct subpool {
  struct subpool *next;
  union node_free_list nodes[SUBPOOL_SIZE];
};

struct subpool *new_subpool(struct subpool *next)
{
  struct subpool *pool = malloc(sizeof *pool);
  if (!pool) return 0;

  // chain sub-pools
  pool->next = next;

  // chain free node-slots
  for (size_t i = 0; i < SUBPOOL_SIZE - 1; i++)
    pool->nodes[i].next_free = &pool->nodes[i + 1];
  pool->nodes[SUBPOOL_SIZE - 1].next_free = 0;

  return pool;
}

struct node_pool {
  union node_free_list *next_free;
  struct subpool *subpools;
};

struct node_pool *new_pool(void)
{
  struct node_pool *pool = malloc(sizeof *pool);
  if (!pool) return 0;

  struct subpool *subpool = new_subpool(0);
  if (!subpool) { free(pool); return 0; }

  pool->subpools = subpool;
  pool->next_free = &subpool->nodes[0];
  return pool;
}

struct node *node_alloc(struct node_pool *pool)
{
  if (pool->next_free == 0) {
    struct subpool *new_top = new_subpool(pool->subpools);
    if (!new_top) return 0;

    // Success, so add new pool to list
    new_top->next = pool->subpools;
    pool->next_free = &new_top->nodes[0];
  }
  struct node *node = &pool->next_free->node;
  pool->next_free = pool->next_free->next_free;
  return node;
}

void free_node(struct node_pool *pool, struct node *node)
{
  union node_free_list *free_list =
              (union node_free_list *)node;
  free_list->next_free = pool->next_free;
  pool->next_free = free_list;
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
  assert(n2 - n1 == 1);
  assert(n3 - n2 == 1);
  // Technically they could sit like that, but it is highly
  // unlikely...
  struct node *n4 = node_alloc(pool);
  assert(n4 - n3 != 1);
  struct node *n5 = node_alloc(pool);
  assert(n5 - n4 == 1);
  struct node *n6 = node_alloc(pool);
  assert(n6 - n5 == 1);
  struct node *n7 = node_alloc(pool);
  assert(n7 - n6 != 1);
  struct node *n8 = node_alloc(pool);
  assert(n8 - n7 == 1);
  struct node *n9 = node_alloc(pool);
  assert(n9 - n8 == 1);
  struct node *n10 = node_alloc(pool);
  assert(n10 - n9 != 1);

  free_node(pool, n2);
  free_node(pool, n10);
  struct node *reused10 = node_alloc(pool);
  assert(n10 == reused10);
  struct node *reused2 = node_alloc(pool);
  assert(n2 == reused2);

  free_pool(pool);

  return 0;
}
