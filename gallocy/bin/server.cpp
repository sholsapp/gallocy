#include <unistd.h>

#include <cstdlib>
#include <ctime>
#include <iostream>

#include "gallocy/entrypoint.h"

int main(int argc, char *argv[]) {
  // TODO(sholsapp): Transparently inject this.
  initialize_gallocy_framework(argv[1]);
  //
  // Application start.
  //
  while (true) {
    int size = 8092 - std::rand() % 8092;
    char *memory = reinterpret_cast<char *>(malloc(sizeof(char) * size));
    LOG_APP("allocated " << size << " byte(s) in " << reinterpret_cast<void *>(memory));
    memset(memory, '!', size);
    free(memory);
    int duration = 30 - std::rand() % 30;
    LOG_APP("sleeping for " << duration);
    sleep(duration);
  }
  //
  // Application end.
  //
  // TODO(sholsapp): Transparently inject this.
  teardown_gallocy_framework();
  return 0;
}
