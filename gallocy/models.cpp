#include "gallocy/models.h"


Engine e;
Model<PeerInfo> peer_info_table(&e);


void Engine::execute(const gallocy::string &sql) {
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


const gallocy::string PeerInfo::TABLE_NAME = "peer_info";

const gallocy::string PeerInfo::CREATE_STATEMENT = \
  "CREATE TABLE peer_info (" \
  "id INTEGER PRIMARY KEY, " \
  "ip_address INT UNIQUE, " \
  "first_seen INT NOT NULL, " \
  "last_seen INT NOT NULL, " \
  "is_master INT NOT NULL);";

const gallocy::string PeerInfo::COLUMNS_LIST = \
  "id, ip_address, first_seen, last_seen, is_master";

gallocy::string PeerInfo::insert() {
  gallocy::stringstream sql;
  sql << "INSERT INTO peer_info "
      << "(ip_address,first_seen,last_seen,is_master) "
      << "VALUES ("
      << ip_address << ","
      << first_seen << ","
      << last_seen << ","
      << is_master
      << ");";
  return sql.str();
}
