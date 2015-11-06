#ifndef GALLOCY_CONSTANTS_H_
#define GALLOCY_CONSTANTS_H_

#define PAGE_SZ 4096

#include <unistd.h>


extern intptr_t &global_main();
extern intptr_t &global_end();
extern intptr_t &global_base();

#endif  // GALLOCY_CONSTANTS_H_
