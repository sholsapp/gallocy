/**
 * An example of a simple SQLite program that creates a database, creates a
 * single table, inserts some sample data, and lastly, selects the data back.
 *
 * Source adopted from http://www.tutorialspoint.com/sqlite/sqlite_c_cpp.htm.
 * Note included here is an update and delete example.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

#include <map>


std::map<void*, int> xSizeMap;


void* xMalloc(int sz) {
  if (sz == 0) {
    return NULL;
  }
  void* ptr = malloc(sz);
  xSizeMap.insert(std::pair<void*, int>(ptr, sz));
  fprintf(stderr, "%p = xMalloc(%d)\n", ptr, sz);
  return ptr;
}

void xFree(void* ptr) {
  fprintf(stderr, "xFree(%p)\n", ptr);
  xSizeMap.erase(ptr);
  free(ptr);
  return;
}

void* xRealloc(void* ptr, int sz) {
  void* new_ptr = realloc(ptr, sz);
  fprintf(stderr, "%p = xRealloc(%p, %d)\n", new_ptr, ptr, sz);
  xSizeMap.insert(std::pair<void*, int>(new_ptr, sz));
  return new_ptr;
}

int xSize(void* ptr) {
  int sz = xSizeMap[ptr];
  fprintf(stderr, "%d = xSize(%p)\n", sz, ptr);
  return sz;
}

int xRoundup(int sz) {
  fprintf(stderr, "xRoundup(%d)\n", sz);
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
  fprintf(stdout, "Initializing sqlite memory configuration!\n");
  if (sqlite3_config(SQLITE_CONFIG_MALLOC, &my_mem) != SQLITE_OK) {
    fprintf(stdout, "Failed to set custom sqlite memory allocator!\n");
  }
  fprintf(stdout, "Success!\n");
}

static int create_callback(void *NotUsed, int argc, char **argv, char **azColName){
   int i;
   for(i=0; i<argc; i++) {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}


static int insert_callback(void *NotUsed, int argc, char **argv, char **azColName){
   int i;
   for(i=0; i<argc; i++) {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}


static int select_callback(void *data, int argc, char **argv, char **azColName){
   int i = 0;
   fprintf(stderr, "%s: ", (const char*)data);
   for(i=0; i<argc; i++) {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}


int main(int argc, char* argv[]) {
   sqlite3 *db;
   char *zErrMsg = 0;
   int  rc;
   char *sql;

   init_sqlite_memory();

   /* Open database */
   rc = sqlite3_open("test.db", &db);
   if(rc) {
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      exit(0);
   } else {
      fprintf(stdout, "Opened database successfully\n");
   }

   /* Create SQL statement */
   sql = "CREATE TABLE COMPANY("            \
         "ID INT PRIMARY KEY     NOT NULL," \
         "NAME           TEXT    NOT NULL," \
         "AGE            INT     NOT NULL," \
         "ADDRESS        CHAR(50),"         \
         "SALARY         REAL );";

   /* Execute SQL statement */
   rc = sqlite3_exec(db, sql, create_callback, 0, &zErrMsg);
   if(rc != SQLITE_OK) {
   fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   } else {
      fprintf(stdout, "Table created successfully\n");
   }

   /* Create SQL statement */
   sql = "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY) " \
         "VALUES (1, 'Paul', 32, 'California', 20000.00 ); " \
         "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY) " \
         "VALUES (2, 'Allen', 25, 'Texas', 15000.00 ); "     \
         "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY)"  \
         "VALUES (3, 'Teddy', 23, 'Norway', 20000.00 );"     \
         "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY)"  \
         "VALUES (4, 'Mark', 25, 'Rich-Mond ', 65000.00 );";

   /* Execute SQL statement */
   rc = sqlite3_exec(db, sql, insert_callback, 0, &zErrMsg);
   if(rc != SQLITE_OK) {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   } else {
      fprintf(stdout, "Records created successfully\n");
   }

   const char* data = "Callback function called";

   /* Create SQL statement */
   sql = "SELECT * from COMPANY";

   /* Execute SQL statement */
   rc = sqlite3_exec(db, sql, select_callback, (void*)data, &zErrMsg);
   if(rc != SQLITE_OK) {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   } else {
      fprintf(stdout, "Operation done successfully\n");
   }

   sqlite3_close(db);

   return 0;
}
