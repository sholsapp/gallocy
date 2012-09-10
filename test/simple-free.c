#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  fprintf(stdout, "> start\n");
  char* ptr1 = (char*) malloc (sizeof(char) * 16);
  char* ptr2 = (char*) malloc (sizeof(char) * 16);
  free(ptr1);
  free(ptr2);
  char* ptr3 = (char*) malloc (sizeof(char) * 16);
  char* ptr4 = (char*) malloc (sizeof(char) * 16);
  free(ptr3);
  free(ptr4);
  fprintf(stdout, "> end\n");
  return 0;
}
