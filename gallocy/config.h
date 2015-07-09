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

typedef gallocy::map<int, int> foo;


gallocy::string read_file(const char*);
gallocy::string get_me(void);
peer_list_t get_peers(void);
