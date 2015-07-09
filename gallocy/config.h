#ifndef _CONFIG_H
#define _CONFIG_H

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <streambuf>
#include <sstream>

#include "frozen.h"
#include "libgallocy.h"


typedef gallocy::vector<gallocy::string> peer_list_t;


gallocy::string read_file(const char*);
void read_config(gallocy::string &, peer_list_t &);
gallocy::string get_me(void);
peer_list_t get_peers(void);

#endif
