#include <string>
#include <vector>

#include "gallocy/request.h"
#include "gallocy/stringutils.h"


/**
 * Create a HTTP request object from the raw request.
 *
 * :param raw: The raw request string.
 */
Request::Request(gallocy::string raw) {
  // Parse the raw request into lines
  gallocy::vector<gallocy::string> lines;
  utils::split(raw, '\n', lines);

  // Parse the first line into the request line parts
  gallocy::vector<gallocy::string> request_line;
  utils::split(lines[0], ' ', request_line);
  method = utils::trim(request_line[0]);
  uri = utils::trim(request_line[1]);
  protocol = utils::trim(request_line[2]);

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
    raw_body = utils::trim(*it);
  }
}


/**
 * The JSON body if present.
 *
 * :returns: The JSON body.
 */
gallocy::json &Request::get_json() {
  if (json == gallocy::json(nullptr)
      && headers.count("Content-Type")
      && headers["Content-Type"] == "application/json") {
    json = gallocy::json::parse(raw_body.c_str());
  }
  return json;
}


/**
 * The request parameters if present.
 *
 * :returns: The request parameters.
 */
Request::Parameters &Request::get_params() {
  if (uri.find("?") == std::string::npos)
    return params;
  gallocy::string param_string = uri.substr(uri.find("?") + 1, uri.length());
  gallocy::vector<gallocy::string> param_parts;
  utils::split(param_string, '&', param_parts);
  for (auto it = std::begin(param_parts); it != std::end(param_parts); ++it) {
    gallocy::vector<gallocy::string> kv;
    utils::split(*it, '=', kv);
    params[kv[0]] = kv[1];
  }
  return params;
}


/**
 * Pretty print a request.
 *
 * Prints directly to standard output.
 */
void Request::pretty_print() {
  std::cout << "Request (" << this << ")" << std::endl
            << "  " << method
            << " " << uri
            << " " << protocol
            << std::endl;
  for (auto &it : headers) {
    std::cout << "  " << it.first
              << ": " << it.second
              << std::endl;
  }
}


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
