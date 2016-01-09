#include <algorithm>
#include <cstring>
#include <new>

#include "gallocy/libgallocy.h"
#include "gallocy/diff.h"

class Element;

class Element {
 public:
  Element(int x, int y) :
    x(x), y(y), value(-1), traceback(NULL) {}
  int x;
  int y;
  int value;
  Element* traceback;
};


class Cost {
 public:
  static const int MATCH = 1;
  static const int MISMATCH = -2;
  static const int GAP = -1;
};


void print_matrix(Element **matrix, size_t matrix_y, size_t matrix_x) {
  fprintf(stderr, "y dim: %lu\n", matrix_y);
  fprintf(stderr, "x dim: %lu\n", matrix_x);
  for (unsigned int y = 0; y < matrix_y; y++) {
    for (unsigned int x = 0; x < matrix_x; x++) {
      fprintf(stderr, "%d ", matrix[y][x].value);
    }
    fprintf(stderr, "\n");
  }
}


void print_diff(const char *mem1, size_t mem1_len, const char* mem2, size_t mem2_len) {
  fprintf(stderr, "-> %s\n", mem1);
  fprintf(stderr, "-> %s\n", mem2);
  for (unsigned int i = 0; i < mem1_len; i++) {
    if (mem1[i] == mem2[i]) {
      // Don't print this, it's noise
    } else if (mem1[i] == '-') {
      fprintf(stderr, "@%d: ", i);
      fprintf(stderr, "+");
      fprintf(stderr, "%c", mem2[i]);
      fprintf(stderr, "\n");
    } else if (mem2[i] == '-') {
      fprintf(stderr, "@%d: ", i);
      fprintf(stderr, "-");
      fprintf(stderr, "%c", mem1[i]);
      fprintf(stderr, "\n");
    } else if (mem1[i] != mem2[i]) {
      fprintf(stderr, "@%d: ", i);
      fprintf(stderr, "-");
      fprintf(stderr, "%c", mem1[i]);
      fprintf(stderr, "+");
      fprintf(stderr, "%c", mem2[i]);
      fprintf(stderr, "\n");
    }
  }
}


int diff(
    const char* mem1, size_t mem1_len, char*& mem1_alignment,
    const char* mem2, size_t mem2_len, char*& mem2_alignment) {

  size_t y_matrix_len = mem1_len + 1;
  size_t x_matrix_len = mem2_len + 1;

  // Allocate the matrices
  Element **_matrix = reinterpret_cast<Element** >(internal_malloc(sizeof(Element *) * y_matrix_len));
  for (unsigned int y = 0; y < y_matrix_len; y++) {
    _matrix[y] = reinterpret_cast<Element *>(internal_malloc(sizeof(Element) * x_matrix_len));
    for (unsigned int x = 0; x < x_matrix_len; x++) {
      Element* e = new (reinterpret_cast<void *>(&_matrix[y][x])) Element(x, y);
      e->value = 0;
    }
  }

  // Initialize the matrices
  _matrix[0][0].value = 0;
  _matrix[0][0].traceback = NULL;

  for (unsigned int x = 1; x < x_matrix_len; x++) {
    _matrix[0][x].value = Cost::GAP * _matrix[0][x].x;
    _matrix[0][x].traceback = &_matrix[0][x-1];
  }

  for (unsigned int y = 1; y < y_matrix_len; y++) {
    _matrix[y][0].value = Cost::GAP * _matrix[y][0].y;
    _matrix[y][0].traceback = &_matrix[y-1][0];
  }

  // Calculate subsolutions
  for (unsigned int y = 1; y < y_matrix_len; y++) {
    for (unsigned int x = 1; x < x_matrix_len; x++) {
      int diag_cost = _matrix[y-1][x-1].value + (
          Cost::MATCH ? mem1[y-1] == mem2[x-1] : Cost::MISMATCH);
      int left_cost = _matrix[y][x-1].value + Cost::GAP;
      int up_cost = _matrix[y-1][x].value + Cost::GAP;

      int arr[3] = {diag_cost, left_cost, up_cost};
      int max_cost = *std::max_element(arr, arr+3);

      if (diag_cost == max_cost)
        _matrix[y][x].traceback = &_matrix[y-1][x-1];
      else if (left_cost == max_cost)
        _matrix[y][x].traceback = &_matrix[y][x-1];
      else if (up_cost == max_cost)
        _matrix[y][x].traceback = &_matrix[y-1][x];
      _matrix[y][x].value = max_cost;
    }
  }

  // Trace an optimal solution back

  int longest = 0;
  Element* cur = &_matrix[y_matrix_len-1][x_matrix_len-1];
  while (cur->traceback != NULL) {
    cur = cur->traceback;
    longest++;
  }

  // Now we can proceed with building the output
  mem1_alignment = reinterpret_cast<char *>(internal_malloc(sizeof(char) * longest));
  mem2_alignment = reinterpret_cast<char *>(internal_malloc(sizeof(char) * longest));
  memset(mem1_alignment, 0, longest);
  memset(mem2_alignment, 0, longest);
  mem1_alignment[longest] = 0;
  mem2_alignment[longest] = 0;

  int _alignment_idx = longest - 1;

  cur = &_matrix[y_matrix_len-1][x_matrix_len-1];
  while (cur != NULL) {
    if (_matrix[cur->y][cur->x].traceback == &_matrix[cur->y-1][cur->x-1]) {
      mem1_alignment[_alignment_idx] = mem1[cur->y-1];
      mem2_alignment[_alignment_idx] = mem2[cur->x-1];
    } else if (_matrix[cur->y][cur->x].traceback == &_matrix[cur->y][cur->x-1]) {
      mem1_alignment[_alignment_idx] = '-';
      mem2_alignment[_alignment_idx] = mem2[cur->x-1];
    } else if (_matrix[cur->y][cur->x].traceback == &_matrix[cur->y-1][cur->x]) {
      mem1_alignment[_alignment_idx] = mem1[cur->y-1];
      mem2_alignment[_alignment_idx] = '-';
    }
    _alignment_idx--;
    cur = cur->traceback;
  }

  // Free the matrices
  for (unsigned int y = 0; y < y_matrix_len; y++) {
    internal_free(_matrix[y]);
  }
  internal_free(_matrix);

  return 0;
}
