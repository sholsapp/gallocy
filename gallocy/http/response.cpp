#include <string>
#include <vector>

#include "gallocy/http/response.h"
#include "gallocy/utils/stringutils.h"


/**
 * Create a response.
 */
Response::Response() {
  protocol = "HTTP/1.0";
}


/**
 * The response as a string.
 *
 * The rseponse as a string, which is appropriate for sending over the wire to
 * an HTTP client.
 *
 * :returns: The response as a string.
 */
gallocy::string Response::str() {
  gallocy::stringstream out;
  out << protocol << " " << status_code << " " << "OK" << "\r\n";
  for (auto it : headers) {
    out << it.first << ": " << it.second << "\r\n";
  }
  out << "\r\n" << body << "\r\n\r\n";
  return out.str();
}


/**
 * The size of the response.
 *
 * The size of the response, which is appropriate for use when the response's
 * size is needed to send over the wire.
 *
 * :returns: The size of the response in bytes.
 */
uint64_t Response::size() {
  return str().size();
}

/**
 * Print the response as a string.
 */
std::ostream& operator<<(std::ostream& stream, const Response& response) {
  return stream << "Response(" << response.status_code << ")";
}
