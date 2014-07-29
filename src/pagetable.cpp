#include <stdio.h>
#include <sqlite3.h>

#include "pagetable.h"


//typedef struct sqlite3_mem_methods sqlite3_mem_methods;
//struct sqlite3_mem_methods {
//  void *(*xMalloc)(int);         /* Memory allocation function */
//  void (*xFree)(void*);          /* Free a prior allocation */
//  void *(*xRealloc)(void*,int);  /* Resize an allocation */
//  int (*xSize)(void*);           /* Return the size of an allocation */
//  int (*xRoundup)(int);          /* Round up request size to allocation size */
//  int (*xInit)(void*);           /* Initialize the memory allocator */
//  void (*xShutdown)(void*);      /* Deinitialize the memory allocator */
//  void *pAppData;                /* Argument to xInit() and xShutdown() */
//};

// TODO(sholsapp): Use a custom memory allocator here that is seperate than
// the one that we expose to the application. SQLite is going to be used as a
// home for the distributed application's page table, so we don't want to mix
// it up with application memory.
sqlite3_mem_methods my_mem = {
  0,
  0,
  0,
  0,
  0,
  0,
  0
};


void init_sqlite_memory() {
  if (sqlite3_config(SQLITE_CONFIG_MALLOC, &my_mem) != SQLITE_OK) {
    fprintf(stderr, "Failed to set custom sqlite memory allocator!\n");
  }
}
