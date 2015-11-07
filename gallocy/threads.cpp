#include "gallocy/threads.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>


/**
 * Page align a pointer.
 *
 * :param p: The pointer to align to the lower page boundary.
 * :returns: The pointer aligned to the lower page boundary.
 */
void* page_align_ptr(void* p) {
  return reinterpret_cast<unsigned char*>((((intptr_t) p) & ~(PAGE_SZ-1)));
}


/**
 * Allocate a stack.
 *
 * Allocate a stack at ``location`` that is ``stack_size`` many pages large.
 *
 * In addition to the actual stack, an addition two pages of memory are
 * allocated and used as guard pages before and after the region of memory
 * returned by this function.
 *
 * :param location: The location or allocate the stack at or ``NULL`` if glibc
 *   should pick.
 * :param stack_size: The size of the stack to request in pages.
 * :returns: A pointer to the lowest addressable byte of the stack buffer.
 */
void* allocate_thread_stack(void* location, size_t stack_size) {
  char* raw = NULL;
  if ((raw = reinterpret_cast<char *>(mmap(location,
      (stack_size + 2) * PAGE_SZ,
      PROT_READ | PROT_WRITE,
      MAP_SHARED | MAP_ANON,
      // TODO(sholsapp): When we can actually pick where to allocate stacks we
      // can use the ``MAP_FIXED`` flag. Until then, we'll let glibc pick.
      //  MAP_SHARED | MAP_ANON | MAP_FIXED,
      -1, 0))) == MAP_FAILED) {
    perror("allocate_thread_stack's mmap");
  }
  memset(raw, 'G', PAGE_SZ);
  if (mprotect(raw, PAGE_SZ - 1, PROT_NONE) == -1)
    perror("Failed to set bottom guard page.");
  memset(raw + PAGE_SZ * (stack_size + 1), 'G', PAGE_SZ);
  if (mprotect(raw + PAGE_SZ * (stack_size + 1), PAGE_SZ, PROT_NONE) == -1)
    perror("Failed to set top guard page.");
  // Even though stacks grow downward, return the lowest addressable byte
  // because that's what the pthread API uses.
  return reinterpret_cast<void *>(raw + PAGE_SZ);
}
