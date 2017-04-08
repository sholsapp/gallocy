#include <string>
#include <vector>

#include "gallocy/http/response.h"
#include "gallocy/utils/stringutils.h"


/**
 * Create a response.
 */
gallocy::http::Response::Response() {
  protocol = "HTTP/1.0";
}

void gallocy::http::Response::from_buffer(gallocy::string raw) {
  // Parse the raw response into lines
  gallocy::vector<gallocy::string> lines;
  utils::split(raw, '\n', lines);

  // Parse the first line into the response line parts
  gallocy::vector<gallocy::string> response_line;
  utils::split(lines[0], ' ', response_line);
  protocol = utils::trim(response_line[0]);
  status_code = std::stoi(utils::trim(response_line[1]));

  // Track the line each parser leaves off.
  uint64_t line_idx = 1;

  // Parse the headers
  for (auto it = std::begin(lines) + 1; it != std::end(lines); ++it) {
    if ((*it).compare("\r") == 0 || (*it).compare("") == 0)
      break;
    gallocy::vector<gallocy::string> header_parts;
    utils::split(*it, ':', header_parts);
    headers[utils::trim(header_parts[0])] = utils::trim(header_parts[1]);
    line_idx++;
  }
  // Skip the '\r' character that indicated end of request head
  line_idx++;

  for (auto it = std::begin(lines) + line_idx; it != std::end(lines); ++it) {
    if ((*it).compare("\r") == 0 || (*it).compare("") == 0)
      break;
    body = utils::trim(*it);
  }
}
/**
 * The response as a string.
 *
 * The rseponse as a string, which is appropriate for sending over the wire to
 * an HTTP client.
 *
 * :returns: The response as a string.
 */
gallocy::string gallocy::http::Response::str() {
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
uint64_t gallocy::http::Response::size() {
  return str().size();
}

/**
 * Print the response as a string.
 */
std::ostream& operator<<(std::ostream& stream, const gallocy::http::Response& response) {
  return stream << "gallocy::http::Response(" << response.status_code << ")";
}
