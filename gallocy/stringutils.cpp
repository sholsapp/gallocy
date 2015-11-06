#include "./stringutils.h"

#include <functional>
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

}  // namespace utils
