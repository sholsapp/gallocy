#ifndef GALLOCY_THREADS_H_
#define GALLOCY_THREADS_H_

#include <stdint.h>
#include <sys/mman.h>


#define PAGE_SZ 4096


void* page_align_ptr(void *p);
void* allocate_thread_stack(void *location, size_t stack_size);


#endif  // GALLOCY_THREADS_H_
