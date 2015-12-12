#include "gallocy/models.h"


const gallocy::string PeerInfo::CREATE_STATEMENT = \
  "CREATE TABLE peer_info (" \
  "id INT PRIMARY KEY ASC, " \
  "ip_address INT NOT NULL, " \
  "last_seen  INT NOT NULL, " \
  "is_master  INT NOT NULL);";

