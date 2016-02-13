#ifndef GALLOCY_CONSTANTS_H_
#define GALLOCY_CONSTANTS_H_

#define PAGE_SZ 4096

#include <unistd.h>
#include <stdint.h>


extern uint64_t *&global_main();
extern uint64_t *&global_end();
extern uint64_t *&global_base();

#endif  // GALLOCY_CONSTANTS_H_
