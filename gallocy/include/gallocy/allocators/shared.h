#ifndef GALLOCY_ALLOCATORS_SHARED_H_
#define GALLOCY_ALLOCATORS_SHARED_H_

#include "gallocy/heaplayers/shared.h"

extern HL::SharedPageTableHeap shared_page_table;

void* sqlite_malloc(int sz);
void sqlite_free(void* ptr);
void* sqlite_realloc(void* ptr, int sz);
int sqlite_size(void* ptr);
int sqlite_roundup(int sz);
int sqlite_init(void* ptr);
void sqlite_shutdown(void* ptr);

#endif  // GALLOCY_ALLOCATORS_SHARED_H_
