#include "config.h"


/**
 * Read the file at path.
 *
 * :param path: The path to the file to read.
 * :return: The entire file as a string.
 */
gallocy::string read_file(const char* path) {
  std::ifstream t(path, std::ifstream::in);
  gallocy::string s((std::istreambuf_iterator<char>(t)),
               std::istreambuf_iterator<char>());
  return s;
}
