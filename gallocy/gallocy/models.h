#ifndef GALLOCY_MODELS_H_
#define GALLOCY_MODELS_H_


#include <iostream>

#include "gallocy/libgallocy.h"


class Engine {
 public:
  Engine() {
    int  rc;
    rc = sqlite3_open(":memory:", &db);
    if (rc) {
      std::cout << "Failed to create database engine!" << std::endl;
    }
  }

  void create_table(gallocy::string sql) {
    int  rc;
    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2(db, sql.c_str(), sql.size(), &stmt, NULL);

    if (rc != SQLITE_OK) {
      std::cout << "Failed to prepare create statement: " << sql << std::endl;
      return;
    }

    if (sqlite3_step(stmt) != SQLITE_DONE) {
      std::cout << "Failed to execute create: " << sql << std::endl;
    }
    sqlite3_finalize(stmt);
    return;
  }

  void execute(gallocy::string sql) {
    int rc;
    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2(db, sql.c_str(), sql.size(), &stmt, NULL);
    if (rc != SQLITE_OK) {
      std::cout << "Failed to prepare execute" << std::endl;
       return;
    }

    if (sqlite3_step(stmt) != SQLITE_DONE) {
      std::cout << "Failed to execute query: " << sql << std::endl;
      return;
    }

    sqlite3_finalize(stmt);

    return;
  }

 public:
  sqlite3 *db;
};


class Model {
};


class PeerInfo : public Model {
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

  gallocy::string insert() {
    gallocy::stringstream sql;
    sql << "INSERT INTO peer_info "
        << "(ip_address,last_seen,is_master) "
        << "VALUES ("
        << ip_address << ","
        << last_seen << ","
        << is_master
        << ");";
    return sql.str();
  }

 public:
  bool is_master;
  uint64_t id;
  uint64_t ip_address;
  uint64_t last_seen;
  static const gallocy::string CREATE_STATEMENT;
};


#endif  // GALLOCY_MODELS_H_
