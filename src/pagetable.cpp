#include <stdio.h>
#include <sqlite3.h>

#include "pagetable.h"


// These should already be included, but include them again to be explicit.
#include "heaplayers/myhashmap.h"
#include "heaplayers/spinlock.h"
#include "heaplayers/lockedheap.h"
#include "heaplayers/freelistheap.h"
#include "heaplayers/firstfitheap.h"
#include "heaplayers/zoneheap.h"
#include "heaplayers/source.h"
#include "heaplayers/stl.h"


class xSizeHeap :
  public HL::LockedHeap<HL::SpinLockType, HL::FreelistHeap<HL::ZoneHeap<SingletonHeap, 16384 - 16> > > {};


typedef
  std::map<void*, int,
  std::less<void*>,
  STLAllocator<std::pair<void*, int>, xSizeHeap> >
    xSizeMapType;


xSizeMapType xSizeMap;


class SqliteHeap: public SourceHeap {};


SqliteHeap sqliteHeap;


void* xMalloc(int sz) {
  if (sz == 0) {
    return NULL;
  }
  void* ptr = sqliteHeap.malloc(sz);
  xSizeMap.insert(std::pair<void*, int>(ptr, sz));
  //fprintf(stderr, "%p = xMalloc(%d)\n", ptr, sz);
  return ptr;
}


void xFree(void* ptr) {
  //fprintf(stderr, "xFree(%p)\n", ptr);
  xSizeMap.erase(ptr);
  sqliteHeap.free(ptr);
  return;
}


void* xRealloc(void* ptr, int sz) {
  if (ptr == NULL) {
    return sqliteHeap.malloc(sz);
  }
  size_t min_size = sqliteHeap.getSize(ptr);
  void* buf = sqliteHeap.malloc(sz);
  if (buf != NULL) {
    memcpy(buf, ptr, min_size);
    sqliteHeap.free(ptr);
  }
  xSizeMap.insert(std::pair<void*, int>(buf, sz));
  //fprintf(stderr, "%p = xRealloc(%p, %d)\n", buf, ptr, sz);
  return buf;
}


int xSize(void* ptr) {
  int sz = xSizeMap[ptr];
  //fprintf(stderr, "%d = xSize(%p)\n", sz, ptr);
  return sz;
}


int xRoundup(int sz) {
  //fprintf(stderr, "xRoundup(%d)\n", sz);
  return sz;
}


int xInit(void* ptr) {
  fprintf(stderr, "xInit\n");
  return 0;
}


void xShutdown(void* ptr) {
  fprintf(stderr, "xShutdown\n");
  return;
}


sqlite3_mem_methods my_mem = {
  &xMalloc,
  &xFree,
  &xRealloc,
  &xSize,
  &xRoundup,
  &xInit,
  &xShutdown,
  0
};


void init_sqlite_memory() {
  if (sqlite3_config(SQLITE_CONFIG_MALLOC, &my_mem) != SQLITE_OK) {
    fprintf(stderr, "Failed to set custom sqlite memory allocator!\n");
    return;
  }
  fprintf(stderr, "Successfully set custom sqlite memory allocator!\n");
  return;
}


static int create_callback(void *NotUsed, int argc, char **argv, char **azColName){
   int i;
   for(i=0; i<argc; i++) {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}


void init_database() {
  sqlite3 *db;
  char *zErrMsg = 0;
  int  rc;
  char *sql;

  //rc = sqlite3_open("test.db", &db);
  rc = sqlite3_open(":memory:", &db);

  if(rc) {
     fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
     exit(0);
  } else {
     fprintf(stdout, "Opened database successfully\n");
  }

  sql = "CREATE TABLE APPLICATION_PAGE_TABLE("            \
        "ID INT PRIMARY KEY     NOT NULL," \
        "ADDRESS        INT     NOT NULL );";

  rc = sqlite3_exec(db, sql, create_callback, 0, &zErrMsg);
  if(rc != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  } else {
    fprintf(stdout, "Table created successfully\n");
  }

}


class Control {
  public:
    Control() {
      init_sqlite_memory();
      init_database();
    }
};

Control c;
