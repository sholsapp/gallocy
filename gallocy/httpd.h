#include <cstdio>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/wait.h>
#include <stdlib.h>

#include "libgallocy.h"

#define ISspace(x) isspace((int)(x))

#define SERVER_STRING "Server: gallocy-httpd/0.1.0\r\n"

int get_line(int, char *, int);
int startup(u_short *);
void *accept_request(void *);
void add_header(int, const char *, const char *);
void cat(int, FILE *);
void error_die(const char *);
void headers(int, const char *);
void serve_file(int, const char *);

void init(void);
void admin(void*);


static inline gallocy::vector<gallocy::string> &split(const gallocy::string &s, char delim, gallocy::vector<gallocy::string> &elems) {
    gallocy::stringstream ss;
    // TODO: Why can't we use the initialization constructor? Compiler
    // complains that it is missing even though std::basic_stringstream is the
    // parent class.
    ss << s;
    gallocy::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>


// trim from start
static inline gallocy::string &ltrim(gallocy::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
}

// trim from end
static inline gallocy::string &rtrim(gallocy::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
}

// trim from both ends
static inline gallocy::string &trim(gallocy::string &s) {
        return ltrim(rtrim(s));
}


/**
 * A HTTP request.
 */
class Request {

  public:

    /**
     * Headers are a simple string to string map.
     */
    typedef gallocy::map<
      gallocy::string, gallocy::string>
      Headers;

    /**
     * Create a HTTP request object from the raw request.
     *
     * :param raw: The raw request string.
     */
    Request(gallocy::string raw) : raw(raw) {

      // Parse the raw request into lines
      gallocy::vector<gallocy::string> lines;
      split(raw, '\n', lines);

      // Parse the first line into the request line parts
      gallocy::vector<gallocy::string> request_line;
      split(lines[0], ' ', request_line);
      method = trim(request_line[0]);
      path = trim(request_line[1]);
      protocol = trim(request_line[2]);

      // Parse the headers
      for (auto it = std::begin(lines) + 1; it != std::end(lines); ++it) {
        if ((*it).compare("\r") == 0)
          break;
        gallocy::vector<gallocy::string> header_parts;
        split(*it, ':', header_parts);
        headers[trim(header_parts[0])] = trim(header_parts[1]);
      }

      // Parse the body
      // TODO

    };

    gallocy::string method;
    gallocy::string path;
    gallocy::string protocol;
    Headers headers;
    int body;

  private:
    gallocy::string raw;

};
