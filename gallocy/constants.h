#include <unistd.h>

#define PAGE_SZ 4096

extern intptr_t& global_main();
extern intptr_t& global_end();
extern intptr_t& global_base();
