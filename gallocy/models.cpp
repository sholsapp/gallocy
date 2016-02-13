#include "gallocy/models.h"
#include "gallocy/utils/logging.h"


// TODO(sholsapp): Rename this to "engine" to be clearer outside of this
// module -- it's polluting the code.
Engine e;
Model<PeerInfo> peer_info_table(&e);


int Engine::execute(const gallocy::string &sql) {
  int rc;
  sqlite3_stmt* stmt;
  rc = sqlite3_prepare_v2(db, sql.c_str(), sql.size(), &stmt, NULL);
  if (rc != SQLITE_OK) {
    LOG_WARNING("Failed to prepare query: " << sql.c_str() << std::endl);
    return -1;
  }
  if (sqlite3_step(stmt) != SQLITE_DONE) {
    LOG_WARNING("Failed to execute query: " << sql << std::endl);
    return -1;
  }
  sqlite3_finalize(stmt);
  return 1;
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
