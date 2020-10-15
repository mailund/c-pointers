#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct gap_buf {
  size_t size;
  size_t cursor;
  size_t gap_end;
  char *buffer;
};

struct gap_buf *new_buffer(size_t init_size)
{
  struct gap_buf *buf = malloc(sizeof *buf);
  if (!buf) return 0;
  buf->buffer = malloc(init_size);
  if (!buf->buffer) {
    free(buf);
    return 0;
  }
  buf->size = init_size;
  buf->cursor = 0;
  buf->gap_end = init_size;
  return buf;
}

void free_buffer(struct gap_buf *buf)
{
  free(buf->buffer);
  free(buf);
}

int insert_character(struct gap_buf *buf, char c)
{
  if (buf->cursor == buf->gap_end) {
    if (buf->size == SIZE_MAX) return 0;
    size_t new_size =
      (buf->size < SIZE_MAX / 2) ? (2 * buf->size) : SIZE_MAX;

    // Allocate a larger buffer
    char *new_buf = realloc(buf->buffer, new_size);
    if (!new_buf) return 0;
    buf->buffer = new_buf;

    // Move the segment to the right of the cursor
    size_t right_size = buf->size - buf->gap_end;
    memmove(buf->buffer + new_size - right_size,
            buf->buffer + buf->gap_end,
            right_size);

    // Update the bookkeeping
    buf->size = new_size;
    buf->gap_end = new_size - right_size;
  }
  
  buf->buffer[buf->cursor++] = c;
  return 1; // success
}

void cursor_left(struct gap_buf *buf)
{
  if (buf->cursor > 0) {
    buf->buffer[--buf->gap_end] =
        buf->buffer[--buf->cursor];
  }
}

void cursor_right(struct gap_buf *buf)
{
  if (buf->gap_end < buf->size) {
    buf->buffer[buf->cursor++] =
      buf->buffer[buf->gap_end++];
  }
}

void backspace(struct gap_buf *buf)
{
  if (buf->cursor > 0)
    buf->cursor--;
}

void delete(struct gap_buf *buf)
{
  if (buf->gap_end < buf->size)
    buf->gap_end++;
}

char *extract_text(struct gap_buf *buf)
{
  size_t occupied = buf->cursor + (buf->size - buf->gap_end);
  // Yes, it is insanely unlikely to happen, but if it does
  // then we do not have space for the zero terminal...
  if (SIZE_MAX == occupied) return 0;

  char *string = malloc(occupied + 1);
  if (!string) return 0;

  strncpy(string, buf->buffer, buf->cursor);
  strncpy(string + buf->cursor,
          buf->buffer + buf->gap_end,
          buf->size - buf->gap_end);
  string[occupied] = '\0';
  return string;
}


int main(void)
{
  struct gap_buf *buf = new_buffer(2);
  if (!buf) {
    perror("Couldn't allocate buffer");
    exit(1);
  }

  char *x = 0;

  insert_character(buf, 'f');
  insert_character(buf, 'o');
  insert_character(buf, 'o');
  x = extract_text(buf);
  printf("%s\n", x);
  free(x);

  cursor_left(buf);
  insert_character(buf, 'x');
  x = extract_text(buf);
  printf("%s\n", x);
  free(x);
  cursor_left(buf);
  cursor_left(buf);
  insert_character(buf, 'y');
  cursor_right(buf);
  cursor_right(buf);
  cursor_right(buf);
  cursor_right(buf);
  insert_character(buf, 'z');
  x = extract_text(buf);
  printf("%s\n", x);
  free(x);

  backspace(buf);
  x = extract_text(buf);
  printf("%s\n", x);
  free(x);
  cursor_left(buf);
  cursor_left(buf);
  delete(buf);
  x = extract_text(buf);
  printf("%s\n", x);
  free(x);

  free_buffer(buf);

  return 0;
}
