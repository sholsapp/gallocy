/**
 * Think about making a memory replay feature to see how process uses heap.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  fprintf(stdout, "> start\n");

  int size = atoi(argv[1]);
  char* ptr_list[size];

  for (int i = 0; i < size; i++) {
    ptr_list[i] = (char*) malloc(sizeof(char) * 1024);
  }

  for (int i = 0; i < size; i++) {
    free(ptr_list[i]);
  }

  fprintf(stdout, "> end\n");
  return 0;
}
