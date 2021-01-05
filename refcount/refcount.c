#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stddef.h>   // for max_align_t
#include "refcount.h"

struct refcount {
  // We either have a reference count or we are deleting
  // using a stack.
  union { size_t rc; void *stack; };
  // Callback function for user-defined objects
  void (*cleanup)(void *, void *);
  max_align_t user_data[];
};

#define struct_ptr(p,type,member)                  \
    (type *)((char *)p - offsetof(type, member))
#define refcount_mem(p)                           \
    struct_ptr(p, struct refcount, user_data)
#define user_mem(rc)                              \
    (void *)(rc->user_data)

void *rc_alloc(size_t size, void (*cleanup)(void *, void *))
{
  struct refcount *mem = malloc(sizeof *mem + size);
  if (!mem) return 0;

  mem->rc = 1;
  mem->cleanup = cleanup;

  return user_mem(mem);
}

void *incref(void *p)
{
  if (!p) return p; // it makes some code easier if we allow this
  struct refcount *mem = refcount_mem(p);
  mem->rc++;
  return p;
}

void cleanup(struct refcount *stack)
{
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

  struct refcount *mem = refcount_mem(p);
  if (--mem->rc == 0) {
    mem->stack = 0; // probably already is, with rc == 0, but still...
    if (ctx) {
      // Schedule for deletion
      struct refcount *stack = ctx;
      mem->stack = stack->stack;
      stack->stack = mem;
    } else {
      // Start cleanup
      cleanup(mem);
    }
    return 0; // reference is now gone...
  }

  return p;
}
