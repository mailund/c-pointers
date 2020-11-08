#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <sys/mman.h>
void *alloc_code_bock(size_t size)
{
  // allocate page-aligned memory with mmap():
  char *buf = mmap(
    0, size,
    PROT_WRITE, MAP_ANON | MAP_PRIVATE,
    -1, 0
  );
  if (buf == MAP_FAILED) return 0; // NULL for error
  else return buf;
}

void *set_exec_prot(void *buf, size_t size)
{
  int res = mprotect(buf, size, PROT_READ | PROT_EXEC);
  if (res == -1) {
    // munmap can fail, but there is nothing we
    // can do about it here...
    munmap(buf, size);
    return 0;
  }
  return buf;
}

void free_code_block(void *buf, size_t size)
{
  // munmap can fail, but there is nothing we
  // can do about it here...
  munmap(buf, size);
}

int main(void)
{
  // Adds two to its input and returns
  unsigned char code[] = {
    0x8d, 0x47, 0x02,     // lea eax,[rdi+0x2]
    0xc3                  // ret
  };

  // Raw memory...
  void *code_block = alloc_code_bock(sizeof code);
  if (!code_block) abort();
  memcpy(code_block, code, sizeof code);
  code_block = set_exec_prot(code_block, sizeof code);
  if (!code_block) abort();

  int (*f)(int) = (int (*)(int))code_block;
  printf("%d\n", f(3));
  free_code_block(code_block, sizeof code);



  return 0;
}
