#ifndef _HLDEFINES_H_
#define _HLDEFINES_H_

// Define HL_EXECUTABLE_HEAP as 1 if you want that (i.e., you're doing
// dynamic code generation).

#define HL_EXECUTABLE_HEAP 0

#if defined(__GNUC__)

// GNU C

#define NO_INLINE       __attribute__ ((noinline))
#define INLINE          inline
#define MALLOC_FUNCTION __attribute__((malloc))
#define RESTRICT        __restrict__

#else

// All others

#define NO_INLINE
#define INLINE inline
#define MALLOC_FUNCTION
#define RESTRICT

#endif

#endif
