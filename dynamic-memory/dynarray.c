#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

struct dynarray {
  size_t size;
  size_t used;
  int *data;
};

#define da_get(da,i)    (da)->data[(i)]
#define da_set(da,i,x)  (da)->data[(i)] = (x)
#define da_len(da)      (da)->used
#define da_as_array(da) (da)->data

int da_init(struct dynarray *da,
            size_t init_size,
            size_t init_used)
{
  assert(init_size >= init_used);
  da->data = malloc(init_size);
  da->size = (da->data) ? init_size : 0;
  da->used = (da->data) ? init_used : 0;
  return !!da->data;
}

void da_dealloc(struct dynarray *da)
{
  free(da->data);
  da->data = 0;
  da->size = da->used = 0;
}

#define MIN(a,b) ((a) < (b)) ? (a) : (b)
int da_resize(struct dynarray *da,
              size_t new_size)
{
  int *new_data = realloc(da->data, new_size);
  // If we cannot allocate, leave everything
  // as it is, but report an error
  if (!new_data) return 0;
  da->data = new_data;
  da->size = new_size;
  da->used = MIN(da->used, new_size);
  return 1; // success
}

#define capped_dbl_size(s) \
  ((s) < SIZE_MAX / 2) ? (2 * (s)) : SIZE_MAX

int da_append(struct dynarray *da, int val)
{
  if (da->used == da->size) {
    size_t new_size = capped_dbl_size(da->size);
    int resize_success = da_resize(da, new_size);
    if (!resize_success) return 0;
  }
  da->data[da->used++] = val;
  return 1; // success
}

int main(void)
{
  struct dynarray da;

  int success = da_init(&da, 4, 4);
  if (!success) {
    printf("allocation error\n");
  }

  for (int i = 0; i < da_len(&da); i++) {
    da_set(&da, i, i);
  }
  for (int i = 0; i < da_len(&da); i++) {
    printf("%d ", da_get(&da, i));
  }
  printf("\n");

  printf("current length %zu\n", da_len(&da));
  for (int i = 0; i < 10; i++) {
    if (!da_append(&da, 10 * (i + 1))) {
      printf("allocation error");
      break;
    }
  }
  printf("current length %zu\n", da_len(&da));
  for (int i = 0; i < da_len(&da); i++) {
    printf("%d ", da_get(&da, i));
  }
  printf("\n");

  da_dealloc(&da);

  return 0;
}
