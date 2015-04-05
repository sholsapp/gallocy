#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdint.h>
#include <sys/mman.h>


#define PAGE_SZ 4096


void* page_align_ptr(void*);
void* allocate_thread_stack(void*, size_t);
