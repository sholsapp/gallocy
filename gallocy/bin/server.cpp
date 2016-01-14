#include <unistd.h>

#include <cstdlib>
#include <ctime>
#include <iostream>

#include "gallocy/entrypoint.h"

int main(int argc, char *argv[]) {
  // TODO(sholsapp): Find a way... any way... to replace this module's main
  // routine transparently so that we can call this before executing
  // application logic.
  initialize_gallocy_framework(argv[1]);

  // TODO(sholsapp): Implement a tear down function for the gallocy framework.
  return 0;
}
