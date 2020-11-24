#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "refcount.h"

#include <stddef.h>   // for max_align_t
#include <stdalign.h> // for alignof
#define MAXALIGN alignof(max_align_t)

struct refcount {
  // We either have a reference count or we are deleting
  // using a stack.
  union { size_t rc; void *stack; };
  // Callback function for user-defined objects
  void (*cleanup)(void *, void *);
};

// On my machine, I need MAXALIGN to fit the refcount structure.
// The union of size_t and void * takes 8 bytes and the function pointer
// another 8 bytes, and max_align_t has alignment 16.
// You will have to check on your machine as well...
#define REFCOUNT_MEM MAXALIGN

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

  return user_mem(rc);
}

void *incref(void *p)
{
  if (!p) return p; // it makes some code easier if we allow this
  struct refcount *rc = refcount_mem(p);
  rc->rc++;
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

  struct refcount *rc = refcount_mem(p);
  if (--rc->rc == 0) {
    rc->stack = 0; // probably already is, with rc == 0, but still...
    if (ctx) {
      // Schedule for deletion
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
