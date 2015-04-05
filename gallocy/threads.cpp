#include "threads.h"

/**
 * Page align a pointer.
 *
 * :param p: The pointer to align to the lower page boundary.
 */
void* page_align_ptr(void* p) {
  return (unsigned char*) (((intptr_t) p) & ~(PAGE_SZ-1));
}


/**
 * Allocate a stack.
 *
 * Allocate a stack at ``location`` that is ``stack_size`` many pages large. In
 * addition to the actual stack, an addition two pages of memory are allocated
 * and used as guard pages before and after the region of memory returned by
 * this function.
 *
 * :param location: The location or allocate the stack at or ``NULL`` if glibc
 *   should pick.
 * :param stack_size: The size of the stack to request in pages.
 */
void* allocate_thread_stack(void* location, size_t stack_size) {
  char* raw = NULL;
  if ((raw = (char*) mmap(location,
      (stack_size + 2) * PAGE_SZ,
      PROT_READ | PROT_WRITE,
      MAP_SHARED | MAP_ANON,
      // XXX: When we can actually pick where to allocate stacks we
      // can use the ``MAP_FIXED`` flag. Until then, we'll let
      // glibc pick.
      //MAP_SHARED | MAP_ANON | MAP_FIXED,
      -1, 0)) == MAP_FAILED) {
    perror("allocate_thread_stack's mmap");
  }
  mprotect(raw, PAGE_SZ, PROT_NONE);
  mprotect(raw + PAGE_SZ * 3, PAGE_SZ, PROT_NONE);
  return (void*) (raw + PAGE_SZ);
}
