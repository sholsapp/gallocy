#ifndef GALLOCY_MODELS_H_
#define GALLOCY_MODELS_H_


#include <iostream>
#include <vector>

#include "external/sqlite3/sqlite3.h"
#include "gallocy/libgallocy.h"
#include "gallocy/sqlite.h"


/**
 * A engine interfaces with a database.
 *
 * An engine is capable of executing queries against a SQLite database.
 */
class Engine {
 public:
  Engine() {
    init_sqlite_memory();
  }
  Engine(const Engine &) = delete;
  Engine &operator=(const Engine &) = delete;
  void initialize() {
    int  rc;
    rc = sqlite3_open(":memory:", &db);
    if (rc) {
      std::cout << "Failed to create database engine!" << std::endl;
    }
  }
  int execute(const gallocy::string &sql);

 public:
  sqlite3 *db;
};


/**
 * A model represents a tables.
 *
 * A table is a collection of rows that are represented by objects that
 * inherit from the Row base class.
 */
template <typename T>
class Model {
 public:
  explicit Model(const Engine *e) {
    engine = e;
  }
  gallocy::vector<T> all() {
    int rc;
    sqlite3_stmt* stmt;
    gallocy::stringstream sql;
    sql << "SELECT " << T::COLUMNS_LIST << " FROM " << T::TABLE_NAME << ";";
    gallocy::vector<T> rows;
    rc = sqlite3_prepare_v2(engine->db, sql.str().c_str(), sql.str().size(), &stmt, NULL);
    if (rc != SQLITE_OK) {
      std::cout << "Failed to prepare execute: " << sql.str() << std::endl;
      return rows;
    }
    while (sqlite3_step(stmt) == SQLITE_ROW) {
      rows.push_back(T::extract(stmt));
    }
    sqlite3_finalize(stmt);
    return rows;
  }
 public:
  const Engine *engine;
};


/**
 * A model to hold peer connection state.
 */
class PeerInfo {
 public:
  /**
   * Primary constructor.
   */
  PeerInfo(uint64_t ip_address, uint64_t first_seen, uint64_t last_seen, bool
      is_master) :
    is_master(is_master),
    ip_address(ip_address),
    first_seen(first_seen),
    last_seen(last_seen) {}

  /**
   * Internal constructor.
   */
  PeerInfo(uint64_t id, uint64_t ip_address, uint64_t first_seen, uint64_t
      last_seen, bool is_master) :
    is_master(is_master),
    id(id),
    ip_address(ip_address),
    first_seen(first_seen),
    last_seen(last_seen) {}

  gallocy::string insert();
  gallocy::string update(gallocy::string col, gallocy::string value);

  static PeerInfo extract(sqlite3_stmt *stmt) {
    return PeerInfo(
      static_cast<uint64_t>(sqlite3_column_int(stmt, 0)),
      static_cast<uint64_t>(sqlite3_column_int(stmt, 1)),
      static_cast<uint64_t>(sqlite3_column_int(stmt, 2)),
      static_cast<uint64_t>(sqlite3_column_int(stmt, 3)),
      static_cast<bool>(sqlite3_column_int(stmt, 4)));
  }

 public:
  bool is_master;
  uint64_t id;
  uint64_t ip_address;
  uint64_t first_seen;
  uint64_t last_seen;
  static const gallocy::string CREATE_STATEMENT;
  static const gallocy::string COLUMNS_LIST;
  static const gallocy::string TABLE_NAME;
};


/**
 * A model to describe the application.
 */
class ApplicationInfo {
 public:
  /**
   * Primary constructor.
   */
  ApplicationInfo(uint64_t text_region, uint64_t data_region, uint64_t
      main_address, uint64_t heap_region, uint64_t memory_in_use, uint64_t
      threads_in_use) :
    data_region(data_region),
    heap_region(heap_region),
    main_address(main_address),
    memory_in_use(memory_in_use),
    text_region(text_region),
    threads_in_use(threads_in_use) {}

  /**
   * Internal constructor.
   */
  ApplicationInfo(uint64_t id, uint64_t text_region, uint64_t data_region,
      uint64_t main_address, uint64_t heap_region, uint64_t memory_in_use,
      uint64_t threads_in_use) :
    data_region(data_region),
    heap_region(heap_region),
    id(id),
    main_address(main_address),
    memory_in_use(memory_in_use),
    text_region(text_region),
    threads_in_use(threads_in_use) {}

 public:
  uint64_t data_region;
  uint64_t heap_region;
  uint64_t id;
  uint64_t main_address;
  uint64_t memory_in_use;
  uint64_t text_region;
  uint64_t threads_in_use;
  static const gallocy::string CREATE_STATEMENT;
  static const gallocy::string COLUMNS_LIST;
  static const gallocy::string TABLE_NAME;
};


/**
 * A model to describe an application's memory.
 */
class ApplicationMemory {
 public:
  /**
   * Primary constructor.
   */
  ApplicationMemory(bool dirty, uint64_t address, uint64_t coherency_model,
      uint64_t faults, uint64_t owner, uint64_t permissions, uint64_t size) :
    dirty(dirty),
    address(address),
    coherency_model(coherency_model),
    faults(faults),
    owner(owner),
    permissions(permissions),
    size(size) {}

  /**
   * Internal constructor.
   */
  ApplicationMemory(bool dirty, uint64_t address, uint64_t coherency_model,
      uint64_t faults, uint64_t id, uint64_t owner, uint64_t permissions,
      uint64_t size) :
    dirty(dirty),
    address(address),
    coherency_model(coherency_model),
    faults(faults),
    id(id),
    owner(owner),
    permissions(permissions),
    size(size) {}

 public:
  bool dirty;
  uint64_t address;
  uint64_t coherency_model;
  uint64_t faults;
  uint64_t id;
  uint64_t owner;
  uint64_t permissions;
  uint64_t size;
  static const gallocy::string CREATE_STATEMENT;
  static const gallocy::string COLUMNS_LIST;
  static const gallocy::string TABLE_NAME;
};


// TODO(sholsapp): Rename to "engine".
extern Engine e;
extern Model<PeerInfo> peer_info_table;
extern Model<ApplicationInfo> application_info_table;
extern Model<ApplicationMemory> application_memory_table;

#endif  // GALLOCY_MODELS_H_
