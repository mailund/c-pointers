#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/mman.h>
void *create_exec_buf(unsigned char const *code, size_t size)
{
  // allocate page-aligned memory with mmap():
  char *buf = mmap(
    // void *add -- location to map from. NULL means anywhere
    0,
    // size_t len -- number of bytes to allocate
    size,
    // int prot -- memory protection bits; we want to write
    PROT_WRITE,
    // int flags -- the type of mapped object (e.g. files);
    // we use want a private anonynous chunk of memory
    MAP_ANON | MAP_PRIVATE,
    // int fd -- file descriptor of the object we map;
    // we are not mapping anything, so -1
    -1,
    // off_t offset -- offset into the file description;
    // we don't use any offset
    0
  );
  if (buf == MAP_FAILED) return 0; // NULL for error

  memcpy(buf, code, size);

  // Now change memory permission to read/exec with mprotect
  int res = mprotect(
    // void *addr,
    buf,
    // size_t len,
    size,
    // int prot
    PROT_READ | PROT_EXEC
  );
  if (res == -1) {
    // munmap can fail, but there is nothing we
    // can do about it here...
    munmap(buf, size);
    return 0;
  }

  return buf;
}

void free_exec_buf(void *buf, size_t size)
{
  // munmap can fail, but there is nothing we
  // can do about it here...
  munmap(buf, size);
}

#define jit_func(RETTYPE, ...)           \
struct {                                 \
  RETTYPE (*func)(__VA_ARGS__);          \
  size_t size;                           \
}

// __typeof__ necessary here...
// also, assigning a void * to a function pointer
// or back is valid in POSIX but undefined in the C
// standard
#define new_jit_func(J, BUF, SIZE)       \
  (__typeof__(J)){                       \
    .func = (__typeof__(J.func))(BUF),   \
    .size = (SIZE)                       \
  }

#define free_jit_func(J)                    \
  do {                                      \
    free_exec_buf((void *)J.func, J.size);  \
    J.func = 0; J.size = 0;                 \
  } while(0)

typedef jit_func(int, int) int_to_int_func;
int_to_int_func adder(int j) // will make i -> i + j
{
  unsigned char code[] = {
    // lea eax,[rdi+xx] (0x87 because we use 32-bit int now)
    0x8d, 0x87, 0x00, 0x00, 0x00, 0x00,
    // ret
    0xc3
  };
  // the int starts at index 2 and goes in the next four
  // bytes, little endian...
  unsigned char *j_code = code + 2;
  for (int offset = 0; offset < 4; offset++) {
    j_code[offset] = (j >> 8 * offset) & 0xff;
  }

  void *code_addr = create_exec_buf(code, sizeof code);
  int_to_int_func add = new_jit_func(add, code_addr, sizeof code);
  return add;
}

int main(void)
{
  // Adds two to its input and returns
  unsigned char code[] = {
    0x8d, 0x47, 0x02,     // lea eax,[rdi+0x2]
    0xc3                  // ret
  };

  // Raw memory...
  void *code_addr = create_exec_buf(code, sizeof code);
  int (*f)(int) = (int (*)(int))code_addr;
  printf("%d\n", f(3));
  free_exec_buf(code_addr, sizeof code);

  // Using a structure wrapper
  code_addr = create_exec_buf(code, sizeof code);
  jit_func(int, int) add_four =
    new_jit_func(add_four, code_addr, sizeof code);
  printf("%d\n", add_four.func(3));
  free_jit_func(add_four);

  int_to_int_func add2 = adder(2);
  int_to_int_func add5 = adder(5);
  for (int i = 0; i < 5; i++) {
    printf("%d, %d, %d\n", i, add2.func(i), add5.func(i));
  }
  free_jit_func(add2);
  free_jit_func(add5);

  return 0;
}
