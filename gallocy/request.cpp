#include <vector>

#include "./request.h"
#include "./stringutils.h"


/**
 * Create a HTTP request object from the raw request.
 *
 * :param raw: The raw request string.
 */
Request::Request(gallocy::string raw) : raw(raw) {
  // Parse the raw request into lines
  gallocy::vector<gallocy::string> lines;
  utils::split(raw, '\n', lines);

  // Parse the first line into the request line parts
  gallocy::vector<gallocy::string> request_line;
  utils::split(lines[0], ' ', request_line);
  method = utils::trim(request_line[0]);
  path = utils::trim(request_line[1]);
  protocol = utils::trim(request_line[2]);

  // Track the line each parser leaves off.
  uint64_t line_idx = 1;

  // Parse the headers
  for (auto it = std::begin(lines) + 1; it != std::end(lines); ++it) {
    if ((*it).compare("\r") == 0)
      break;
    gallocy::vector<gallocy::string> header_parts;
    utils::split(*it, ':', header_parts);
    headers[utils::trim(header_parts[0])] = utils::trim(header_parts[1]);
    line_idx++;
  }
  // Skip the '\r' character that indicated end of request head
  line_idx++;

  // TODO(sholsapp): parse the body, if any.
  for (auto it = std::begin(lines) + line_idx; it != std::end(lines); ++it) {
    if ((*it).compare("\r") == 0)
      break;
    raw_body = utils::trim(*it);
  }
}
