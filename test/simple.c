#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  fprintf(stdout, "> start\n");
  char* ptr = (char*) malloc (sizeof(char) * 16);
  ptr[0] = 'A';
  for (int i = 0; i < 15; i++) {
    ptr[i] = 'A';
  }
  ptr[15] = 0;
  fprintf(stderr, "Memory says: %s\n", ptr);
  free(ptr);
  fprintf(stdout, "> end\n");
  return 0;
}
