#ifndef _PAGETABLE_H
#define _PAGETABLE_H

#include <string>
#include <sqlite3.h>


void init_sqlite_memory();


class PageTable {

  public:
    PageTable() : database_path("pagetable.db") {
      init_sqlite_memory();
      open_database();
      create_tables();
    }

    void open_database();
    void create_tables();
    void insert_page_table_entry(void* ptr);

    static int noop_callback(void *not_used, int argc, char **argv, char **az_col_name);
    static int print_callback(void *not_used, int argc, char **argv, char **az_col_name);

  private:

    sqlite3 *db;
    char *database_path;

};


extern PageTable pt;


#endif
