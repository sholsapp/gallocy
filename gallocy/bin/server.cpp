#include <pthread.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>


extern int initialize_gallocy_framework(const char* config_path);
extern int teardown_gallocy_framework();


void *work(void *arg) {
  while (true) {
    int size = 8092 - std::rand() % 8092;
    char *memory = reinterpret_cast<char *>(malloc(sizeof(char) * size));
    // LOG_APP("allocated " << size << " byte(s) in " << reinterpret_cast<void *>(memory));
    // std::cout << "allocated " << size << " byte(s) in " << reinterpret_cast<void *>(memory) << std::endl;
    memset(memory, '!', size);
    free(memory);
    int duration = 30 - std::rand() % 30;
    // LOG_APP("sleeping for " << duration);
    sleep(duration);
  }
}


int main(int argc, char *argv[]) {
  // TODO(sholsapp): Transparently inject this.
  initialize_gallocy_framework(argv[1]);
  //
  // Application start.
  //
  pthread_t thread;
  pthread_create(&thread, nullptr, work, nullptr);
  pthread_join(thread, nullptr);
  //
  // Application end.
  //
  // TODO(sholsapp): Transparently inject this.
  teardown_gallocy_framework();
  return 0;
}
