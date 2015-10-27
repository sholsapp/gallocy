#include "libgallocy.h"

/**
 * A HTTP request.
 */
class Request {

  /**
   * HTTP headers are a simple string to string map.
   */
  typedef gallocy::map<
    gallocy::string, gallocy::string>
    Headers;

  public:

    Request(gallocy::string);

    gallocy::string method;
    gallocy::string path;
    gallocy::string protocol;
    Headers headers;
    int body;

  private:
    gallocy::string raw;

};
