#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  fprintf(stdout, "> start\n");
  char* ptr = (char*) malloc (sizeof(char) * 16);
  ptr[0] = 'A';
  free(ptr);
  fprintf(stdout, "> end\n");
  return 0;
}
