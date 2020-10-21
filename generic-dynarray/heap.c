#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#define MIN(a,b) ((a) < (b)) ? (a) : (b)

#define da_at(da,i)  (da->data[(i)])
#define da_len(da)   (da->meta.used)

struct da_meta {
  size_t size;
  size_t used;
};

#define dynarray(TYPE)                 \
struct {                               \
  struct da_meta meta;                 \
  TYPE   data[];                       \
}

#define size_overflow(meta_size, obj_size, len) \
  ((SIZE_MAX - meta_size) / obj_size < len)

// Always free if we cannot reallocate
void *realloc_dynarray_mem(void *p,
                           size_t meta_size,
                           size_t obj_size,
                           size_t new_len)
{
  if (size_overflow(meta_size, obj_size, new_len))
    goto abort;

  struct da_meta *new_da =
    realloc(p, meta_size + obj_size * new_len);
  if (!new_da) goto abort;

  new_da->size = new_len;
  new_da->used = MIN(new_da->used, new_len);

  return new_da;

abort:
  free(p);
  return 0;
}

void *new_dynarray_mem(size_t meta_size,
                       size_t obj_size,
                       size_t len)
{
  struct da_meta *array =
    realloc_dynarray_mem(0, meta_size, obj_size, len);
  if (array) {
    // we do set size in realloc, but
    array->size = len;
    // if used was not initialised in realloc (and it wasn't)
    // then we have to set it here...
    array->used = 0;
  }
  return array;
}

#define new_da(type, init_size)                  \
  new_dynarray_mem(sizeof(dynarray(type)),       \
                   sizeof(type), init_size)

#define da_free(da)                              \
  do { free(da); da = 0; } while(0)

#define grow(size)                               \
  (((size) == 0) ? /* special case for zero */   \
    1 :                                          \
    ((size) > SIZE_MAX / 2) ? /* can we grow? */ \
      0 : /* no, then report size zero */        \
      (2 * (size))) /* double the size */

#define da_append(da, ...)                       \
do {                                             \
  if (da->meta.used == da->meta.size) {          \
    size_t new_size = grow(da->meta.size);       \
    if (new_size == 0) { da_free(da); break; }   \
    da = realloc_dynarray_mem(                   \
      da, sizeof *da, sizeof *da->data, new_size \
    );                                           \
    if (!da) break;                              \
  }                                              \
  da->data[da->meta.used++] = __VA_ARGS__;       \
} while (0)

// clean up after ourselves...
#undef grow

int main(void)
{
  dynarray(int) *int_array = new_da(int, 10);
  if (!int_array) goto error;
  printf("%zu out of %zu\n", int_array->meta.used, int_array->meta.size);

  for (int i = 0; i < 5; i++) {
    da_append(int_array, i);
    if (!int_array) goto error;
  }

  for (int i = 0; i < da_len(int_array); i++) {
    printf("%d ", da_at(int_array, i));
  }
  printf("\n");

  da_free(int_array);

  return 0;

error:
  return 1;
}
