#ifndef GALLOCY_MODELS_H_
#define GALLOCY_MODELS_H_


#include <iostream>
#include <vector>

#include "gallocy/libgallocy.h"


/**
 * A engine interfaces with a database.
 *
 * An engine is capable of executing queries against a SQLite database.
 */
class Engine {
 public:
  Engine() {
    int  rc;
    rc = sqlite3_open(":memory:", &db);
    if (rc) {
      std::cout << "Failed to create database engine!" << std::endl;
    }
  }
  void execute(gallocy::string sql);

 public:
  sqlite3 *db;
};


extern Engine e;


/**
 * A model represents a tables.
 *
 * A table is a collection of rows that are represented by objects that
 * inherit from the Row base class.
 */
template <typename T>
class Model {
 public:
  static gallocy::vector<T> all() {
    int rc;
    sqlite3_stmt* stmt;
    gallocy::stringstream sql;
    sql << "SELECT " << T::COLUMNS_LIST << " FROM " << T::TABLE_NAME << ";";
    gallocy::vector<T> rows;
    rc = sqlite3_prepare_v2(engine.db, sql.str().c_str(), sql.str().size(), &stmt, NULL);
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
  static const Engine engine;
};


template <typename T>
const Engine Model<T>::engine = e;


class PeerInfo {
 public:
  PeerInfo(uint64_t ip_address, uint64_t last_seen, bool is_master) :
    is_master(is_master),
    ip_address(ip_address),
    last_seen(last_seen) {}

  PeerInfo(uint64_t id, uint64_t ip_address, uint64_t last_seen, bool is_master) :
    is_master(is_master),
    id(id),
    ip_address(ip_address),
    last_seen(last_seen) {}

  gallocy::string insert();

  static PeerInfo extract(sqlite3_stmt *stmt) {
    return PeerInfo(
      static_cast<uint64_t>(sqlite3_column_int(stmt, 0)),
      static_cast<uint64_t>(sqlite3_column_int(stmt, 1)),
      static_cast<uint64_t>(sqlite3_column_int(stmt, 2)),
      static_cast<bool>(sqlite3_column_int(stmt, 3)));
  }

 public:
  bool is_master;
  uint64_t id;
  uint64_t ip_address;
  uint64_t last_seen;
  static const gallocy::string CREATE_STATEMENT;
  static const gallocy::string COLUMNS_LIST;
  static const gallocy::string TABLE_NAME;
};


extern Model<PeerInfo> peer_info_table;


#endif  // GALLOCY_MODELS_H_
