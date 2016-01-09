#include "gallocy/stringutils.h"

#include <arpa/inet.h>

#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <streambuf>
#include <vector>


namespace utils {

  /**
   * Left trim whitespace from a string.
   *
   * :param s: The string to left trim.
   * :returns: The left trimmed string.
   */
  gallocy::string &ltrim(gallocy::string &s) {
          s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
          return s;
  }


  /**
   * Right trim whitespace from a string.
   *
   * :param s: The string to right trim.
   * :returns: The right trimmed string.
   */
  gallocy::string &rtrim(gallocy::string &s) {
          s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
          return s;
  }


  /**
   * Trim whitespace from a string.
   *
   * :param s: The string to trim.
   * :returns: The trimmed string.
   */
  gallocy::string &trim(gallocy::string &s) {
          return ltrim(rtrim(s));
  }


  /**
   * Split a string on a character delimiter.
   *
   * :param s: The string to split.
   * :param delim: The delimiter to split on.
   * :param elems: A reference to a vector to add results to.
   * :returns: The reference to the vector containing the
     * split parts.
   */
  gallocy::vector<gallocy::string> &split(const gallocy::string &s, char delim, gallocy::vector<gallocy::string> &elems) {
      gallocy::stringstream ss(s);
      gallocy::string item;
      while (std::getline(ss, item, delim)) {
          elems.push_back(item);
      }
      return elems;
  }


  /**
   * Read the file at path.
   *
   * :param path: The path to the file to read.
   * :return: The entire file as a string.
   */
  gallocy::string read_file(const char *path) {
    std::ifstream t(path, std::ifstream::in);
    gallocy::string s((std::istreambuf_iterator<char>(t)),
                       std::istreambuf_iterator<char>());
    return s;
  }


  /**
   * Convert a string to hex.
   *
   * :param input: The string to convert to hex.
   * :return: The hex representation of the string.
   */
  gallocy::string string_to_hex(const gallocy::string &input) {
      static const char* const lut = "0123456789ABCDEF";
      size_t len = input.length();
      gallocy::string output;
      output.reserve(2 * len);
      for (size_t i = 0; i < len; ++i) {
          const unsigned char c = input[i];
          output.push_back(lut[c >> 4]);
          output.push_back(lut[c & 15]);
      }
      return output;
  }


  /**
   * Check if a string starts with another.
   *
   * :param haystack: The reference string.
   * :param needle: The prefix to look for in the reference string.
   * :returns: True if the reference string starts with the prefix.
   */
  bool startswith(gallocy::string haystack, gallocy::string needle) {
    return needle.length() <= haystack.length()
      && std::equal(needle.begin(), needle.end(), haystack.begin());
  }


  /**
   * Check if a string ends with another.
   *
   * :param haystack: The reference string.
   * :param needle: The postfix to look for in the reference string.
   * :returns: True if the reference string ends with the postfix.
   */
  bool endswith(gallocy::string haystack, gallocy::string needle) {
    return needle.length() <= haystack.length()
      && std::equal(needle.begin(), needle.end(), haystack.end() - needle.size());
  }

  /**
   * Parse an IPv4 internet address from a string.
   *
   * .. note::
   *
   *   This function naively parses an IPv4 internet address from a string.
   *   This function needs to be rewritten such that endian-ness and IPv6 is
   *   handled.
   *
   * :param ip_address: The IPv4 internet address as a string.
   * :returns: An unsigned 64 bit integer of the IPv4 address.
   */
  uint64_t parse_internet_address(const gallocy::string &ip_address) {
    uint64_t ip = 0;
    uint8_t buf[4];
    int r = inet_pton(AF_INET, ip_address.c_str(), buf);
    if (r <= 0) {
      return 0;
    }
    for (int i = 0; i < 4; i++) {
      ip <<= 8;
      ip |= buf[i] & 0xff;
    }
    return ip;
  }
}  // namespace utils
