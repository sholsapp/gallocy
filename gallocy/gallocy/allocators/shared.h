#include "heaplayers/shared.h"

extern HL::SharedPageTableHeap shared_page_table;

void* xMalloc(int sz);
void xFree(void* ptr);
void* xRealloc(void* ptr, int sz);
int xSize(void* ptr);
int xRoundup(int sz);
int xInit(void* ptr);
void xShutdown(void* ptr);
