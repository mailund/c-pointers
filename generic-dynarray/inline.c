#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#define size_check(n,type)                 \
  ((SIZE_MAX / sizeof(type)) >= (n))
#define checked_malloc(n,type)             \
  (size_check((n),(type)) ?                \
    malloc((n) * sizeof(type)) : 0)
#define checked_realloc(p,n,type)          \
  (size_check((n),(type)) ?                \
    realloc((p), (n) * sizeof(type)) : 0)

#define max_array_len(type)               \
  (SIZE_MAX / sizeof(type))

#define at_max_len(n,type)                \
  ((n) == max_array_len(type))

#define capped_dbl(n,type)                \
  (((n) < max_array_len(type) / 2)        \
    ? (2 * (n)) : max_array_len(type))


#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#define MIN(a,b) ((a) < (b)) ? (a) : (b)
#define MIN_ARRAY_SIZE 1

#define da_at(da,i)  ((da).data[(i)])
#define da_len(da)   ((da).used)

#define dynarray(TYPE)                 \
struct {                               \
  size_t size;                         \
  size_t used;                         \
  TYPE *data;                          \
}

#if 0
// version that uses ,-operator so we get a status
// as a return value
#define da_init(da, init_size, init_used)         \
((da).data =                                      \
   checked_malloc(MAX(init_size, MIN_ARRAY_SIZE), \
                  *(da).data),                    \
 (da).size = (da).data ? init_size : 0,           \
 (da).used = (da).data ? init_used : 0,           \
 !!da.data)

#else

// version where we use a status variable
#define da_init(da, status, init_size, init_used)       \
do {                                                    \
 (da).data =                                            \
    checked_malloc(MAX(init_size, MIN_ARRAY_SIZE),      \
                   *(da).data);                         \
 (da).size = (da).data ? init_size : 0;                 \
 (da).used = (da).data ? init_used : 0;                 \
 status = !!da.data;                                    \
} while (0);

#endif

#define da_dealloc(da)                            \
do {                                              \
  free((da).data);                                \
  (da).data = 0;                                  \
  (da).size = (da).used = 0;                      \
} while (0);

#if 0
// typesafe version that uses a non-standard __typeof__ macro

#define da_resize(da, status, new_size)           \
do {                                              \
  size_t alloc_size =                             \
    MAX(new_size, MIN_ARRAY_SIZE);                \
  __typeof__(da.data) new_data =                  \
    checked_realloc((da).data,                    \
        alloc_size, *(da).data);                  \
  if (!new_data) { status = false; break; }       \
  (da).data = new_data;                           \
  (da).size = alloc_size;                         \
  (da).used = MIN((da).used, new_size);           \
  status = true;                                  \
} while (0);

#else
// using void *
#define da_resize(da, status, new_size)           \
do {                                              \
  size_t alloc_size =                             \
    MAX(new_size, MIN_ARRAY_SIZE);                \
  void *new_data = checked_realloc((da).data,     \
          alloc_size, *(da).data);                \
  if (!new_data) { status = false; break; }       \
  (da).data = new_data;                           \
  (da).size = alloc_size;                         \
  (da).used = MIN((da).used, new_size);           \
  status = true;                                  \
} while (0);

#endif

// simple append for simple arguments. It can handle
// da_append(int_da, 13, status);
// but not
// da_append(point_da, (point){.x = 0, .y = 2}, status);

#if 0

#define da_append(da, status, val)              \
do {                                            \
  if ((da).used == (da).size) {                 \
    if (at_max_len((da).size, *(da).data))      \
      { status = false; break; }                \
    size_t new_size =                           \
      capped_dbl((da).size, *(da).data);        \
    da_resize(da, status, new_size);            \
    if (!status) break;                         \
  }                                             \
  (da).data[(da).used++] = val;                 \
  status = true;                                \
} while (0);

#else

#define da_append(da, status, ...)              \
do {                                            \
  if ((da).used == (da).size) {                 \
    if (at_max_len((da).size, *(da).data))      \
      { status = false; break; }                \
    size_t new_size =                           \
      capped_dbl((da).size, *(da).data);        \
    da_resize(da, status, new_size);            \
    if (!status) break;                         \
  }                                             \
  (da).data[(da).used++] = __VA_ARGS__;         \
  status = true;                                \
} while (0);

#endif


typedef struct { double x, y; } point;

#if 0
// call it with a dynarray(point) * and you get an
// incompatible pointer type, because identical
// anonymous structs are not the same type
bool add_origin(dynarray(point) *da)
{
  bool status;
  da_append(*da, status, (point){ .x = 0, .y = 0 });
  return status;
}

#else

typedef dynarray(point) point_array;

bool add_origin(point_array *da)
{
  bool status;
  da_append(*da, status, (point){ .x = 0, .y = 0 });
  return status;
}

#endif

int main(void)
{
  bool status = true;

  dynarray(int) int_da;
  da_init(int_da, status, 0, 0);
  if (status) {
    printf("everything when well.\n");
  }

  dynarray(double) double_da;
  da_init(double_da, status, 0, 0);

  dynarray(point) point_da;
  da_init(point_da, status, 0, 0);

  da_resize(int_da, status, 10);
  printf("status: %d, size %zu, used %zu\n",
         status, int_da.size, int_da.used);
  da_resize(int_da, status, 0);
  printf("status: %d, size %zu, used %zu\n",
         status, int_da.size, int_da.used);

  da_append(int_da, status, 13);
  if (!status) { perror("huh?"); exit(1); }
  da_append(point_da, status, (point){.x = 0, .y = 2});
  if (!status) { perror("huh?"); exit(1); }

  for (int i = 0; i < 5; i++) {
    da_append(int_da, status, i);
    if (!status) { perror("huh?"); exit(1); }
    da_append(double_da, status, (double)i);
    if (!status) { perror("huh?"); exit(1); }
    da_append(point_da, status, (point){ .x = i, .y = -i });
    if (!status) { perror("huh?"); exit(1); }
  }

  for (int i = 0; i < da_len(int_da); i++) {
    printf("%d ", da_at(int_da, i));
  }
  printf("\n");

  for (int i = 0; i < da_len(double_da); i++) {
    printf("%.1f ", da_at(double_da, i));
  }
  printf("\n");

  for (int i = 0; i < da_len(point_da); i++) {
    printf("<%.1f,%.1f> ", da_at(point_da, i).x, da_at(point_da, i).y);
  }
  printf("\n");

  da_dealloc(int_da);
  da_dealloc(double_da);
  da_dealloc(point_da);

  point_array pa;
  da_init(pa, status, 0, 0);
  add_origin(&pa);
  for (int i = 0; i < da_len(pa); i++) {
    printf("<%.1f,%.1f> ", da_at(pa, i).x, da_at(pa, i).y);
  }
  printf("\n");
  da_dealloc(pa);

  return 0;
}
