#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include <stdio.h>
#include <stdalign.h>

struct subpool {
  char *data;
  struct subpool *next;
};

struct pool {
  struct subpool *top_pool;
  size_t block_size;
  size_t top_capacity;
  void *free_ptr;
};

void chain_subpool(char *data, size_t n, size_t block_size)
{
  // We need a void ** so we can write a void * into each block.
  void **p = (void **)data + (n - 1) * block_size;
  *p = 0;
  for (size_t i = 0; i < n; i++) {
    p = (void **)(data + i * block_size);
    *p = data + (i + 1) * block_size;
  }
}

struct subpool *new_subpool(size_t n, size_t block_size,
                              struct subpool *next)
{
  if (SIZE_MAX / n < block_size) return 0; // overflow error

  struct subpool *spool = malloc(sizeof *spool);
  if (!spool) return 0;
  spool->data = malloc(n * block_size);
  if (!spool->data) { free(spool); return 0; }

  spool->next = next;
  chain_subpool(spool->data, n, block_size);

  return spool;
}

#define MAX(a,b) (((a) > (b)) ? (a) : (b))

static inline
size_t aligned_block_size(size_t type_size, size_t type_align)
{
  // The block size must be a multiple of align of size
  // at least min_size.
  size_t min_size   = MAX(sizeof(void *), type_size);
  size_t align      = MAX(alignof(void *), type_align);
  size_t block_size = ((min_size - 1) / align + 1) * align;
  return block_size;
}

struct pool *new_pool_size(size_t init_capacity,
                           size_t type_size, size_t type_align)
{
  size_t block_size = aligned_block_size(type_size, type_align);

  struct pool *pool = malloc(sizeof *pool);
  if (!pool) return 0;
  pool->top_pool = new_subpool(init_capacity, block_size, 0);
  if (!pool->top_pool) { free(pool); return 0; }

  pool->block_size = block_size;
  pool->top_capacity = init_capacity;
  pool->free_ptr = pool->top_pool->data;

  return pool;
}

#define new_pool(init_capacity, type) \
  new_pool_size(init_capacity, sizeof(type), alignof(type))

void free_pool(struct pool *pool)
{
  struct subpool *sp = pool->top_pool;
  while (sp) {
    struct subpool *next = sp->next;
    free(sp->data);
    free(sp);
    sp = next;
  }
  free(pool);
}

void *pool_alloc(struct pool *pool)
{
  if (pool->free_ptr == 0) {
    // Check overflow
    if (SIZE_MAX / 2 / pool->block_size < pool->top_capacity) return 0;
    size_t new_size = 2 * pool->top_capacity;

    struct subpool *new_top =
      new_subpool(new_size, pool->block_size, pool->top_pool);
    if (!new_top) return 0;

    pool->top_pool = new_top;
    pool->top_capacity = new_size;
    pool->free_ptr = new_top->data;
  }
  void *p = pool->free_ptr;
  pool->free_ptr = *(void **)pool->free_ptr;
  return p;
}

void pool_free(struct pool *pool, void *p)
{
  void **free_ptr = (void **)p;
  *free_ptr = pool->free_ptr;
  pool->free_ptr = free_ptr;
}

int main(void)
{
  struct pool *pool = new_pool(3, int);
  int *i1 = pool_alloc(pool); *i1 = 1;
  int *i2 = pool_alloc(pool); *i2 = 2;
  int *i3 = pool_alloc(pool); *i3 = 3;

  printf("%d %d %d\n", *i1, *i2, *i3);
  int *i4 = pool_alloc(pool); *i4 = 4;
  int *i5 = pool_alloc(pool); *i5 = 5;
  int *i6 = pool_alloc(pool); *i6 = 6;
  printf("%d %d %d\n", *i4, *i5, *i6);

  pool_free(pool, i2);
  int *new2 = pool_alloc(pool);
  assert(i2 == new2);

  for (int i = 0; i < 20; i++) {
    pool = new_pool_size(1, i, 1);
    free_pool(pool);
  }

  return 0;
}
