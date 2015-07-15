#include "constants.h"


extern "C" {
extern char* main;
extern char* _end;
}


intptr_t& global_main() {
   static intptr_t m = ((intptr_t) &main);
   return m;
}

intptr_t& global_end() {
   static intptr_t e = ((intptr_t) &_end);
   return e;
}

intptr_t& global_base() {
   // Page align the _end of the program + a page
   static intptr_t base = (((intptr_t) &_end) & ~(PAGE_SZ - 1)) + PAGE_SZ;
   return base;
}
