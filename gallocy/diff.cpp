#include <algorithm>
#include <new>

#include <cstring>


#include "diff.h"


class Element {
public:
  Element(int x, int y) :
    x(x), y(y), value(-1), traceback(-1) {}
  int x;
  int y;
  int value;
  int traceback;
};


class Cost {
public:
  static const int MATCH = 1;
  static const int MISMATCH = -1;
  static const int GAP = -2;
};


class Moves {
public:
  static const int ORIGIN = 100;
  static const int LEFT = 101;
  static const int RIGHT = 102;
  static const int UP = 103;
  static const int DIAG = 104;
};


void print_matrix(Element** matrix, size_t matrix_y, size_t matrix_x) {
  fprintf(stderr, "y dim: %d\n", matrix_y);
  fprintf(stderr, "x dim: %d\n", matrix_x);
  for (int y = 0; y < matrix_y; y++ ) {
    for (int x = 0; x < matrix_x; x++ ) {
      fprintf(stderr, "%d ", matrix[y][x].value);
    }
    fprintf(stderr, "\n");
  }
}


void print_diff(const char* mem1, size_t mem1_len, const char* mem2, size_t mem2_len) {
  fprintf(stderr, "-> %s\n", mem1);
  fprintf(stderr, "-> %s\n", mem2);
  for (int i = 0; i < mem1_len; i++) {
    if (mem1[i] == mem2[i]) {
      // Don't print this, it's noise
    }
    else if (mem1[i] == '-') {
      fprintf(stderr, "@%d: ", i);
      fprintf(stderr, "+");
      fprintf(stderr, "%c", mem2[i]);
      fprintf(stderr, "\n");
    }
    else if (mem2[i] == '-') {
      fprintf(stderr, "@%d: ", i);
      fprintf(stderr, "-");
      fprintf(stderr, "%c", mem1[i]);
      fprintf(stderr, "\n");
    }
    else if (mem1[i] != mem2[i]) {
      fprintf(stderr, "@%d: ", i);
      fprintf(stderr, "-");
      fprintf(stderr, "%c", mem1[i]);
      fprintf(stderr, "+");
      fprintf(stderr, "%c", mem2[i]);
      fprintf(stderr, "\n");
    }
  }
}


char* diff(
    const char* mem1, size_t mem1_len,
    const char* mem2, size_t mem2_len) {

  size_t y_matrix_len = mem1_len + 1;
  size_t x_matrix_len = mem2_len + 1;

  // Allocate the matrices
  Element** _matrix = (Element**) malloc(sizeof(Element*) * y_matrix_len);
  for (int y = 0; y < y_matrix_len; y++) {
    _matrix[y] = (Element*) malloc(sizeof(Element) * x_matrix_len);
    for (int x = 0; x < x_matrix_len; x++) {
      Element* e = new ((void*) &_matrix[y][x]) Element(x, y);
      e->value = 0;
    }
  }

  // Initialize the matrices
  _matrix[0][0].value = 0;
  _matrix[0][0].traceback = Moves::ORIGIN;

  for (int x = 1; x < x_matrix_len; x++) {
    _matrix[0][x].value = Cost::GAP * _matrix[0][x].x;
    _matrix[0][x].traceback = Moves::LEFT;
  }

  for (int y = 1; y < y_matrix_len; y++) {
    _matrix[y][0].value = Cost::GAP * _matrix[y][0].y;
    _matrix[y][0].traceback = Moves::UP;
  }

  // Calculate subsolutions
  for (int y = 1; y < y_matrix_len; y++) {
    for (int x = 1; x < x_matrix_len; x++) {
      int diag_cost = _matrix[y-1][x-1].value + (
          Cost::MATCH ? mem1[y-1] == mem2[x-1] : Cost::MISMATCH);
      int left_cost = _matrix[y][x-1].value + Cost::GAP;
      int up_cost = _matrix[y-1][x].value + Cost::GAP;
      int arr[3] = {0};
      arr[0] = diag_cost;
      arr[1] = left_cost;
      arr[2] = up_cost;
      int max_cost = *std::max_element(arr, arr+3);
      if (diag_cost == max_cost)
        _matrix[y][x].traceback = Moves::DIAG;
      else if (left_cost == max_cost)
        _matrix[y][x].traceback = Moves::LEFT;
      else if (up_cost == max_cost)
        _matrix[y][x].traceback = Moves::UP;
      _matrix[y][x].value = max_cost;
    }
  }

  // Trace an optimal solution back
  int params[2] = {0};
  params[0] = mem1_len;
  params[1] = mem2_len;
  int longest = *std::max_element(params, params+2);

  char* mem1_alignment = (char*) malloc(sizeof(char) * longest + 1);
  char* mem2_alignment = (char*) malloc(sizeof(char) * longest + 1);

  memset(mem1_alignment, '?', longest);
  memset(mem2_alignment, '?', longest);

  int _alignment_idx = longest;
  int _ty = y_matrix_len - 1;
  int _tx = x_matrix_len - 1;

  while (_alignment_idx >= 0) {
    if (_matrix[_ty][_tx].traceback == Moves::DIAG) {
      mem1_alignment[_alignment_idx] = mem1[_ty-1];
      mem2_alignment[_alignment_idx] = mem2[_tx-1];
      _ty--;
      _tx--;
    }
    else if (_matrix[_ty][_tx].traceback == Moves::LEFT) {
      mem1_alignment[_alignment_idx] = '-';
      mem2_alignment[_alignment_idx] = mem2[_tx-1];
      _tx--;
    }
    else if (_matrix[_ty][_tx].traceback == Moves::UP) {
      mem1_alignment[_alignment_idx] = mem1[_ty-1];
      mem2_alignment[_alignment_idx] = '-';
      _ty--;
    }
    else if (_matrix[_ty][_tx].traceback == Moves::ORIGIN) {
      mem1_alignment[_alignment_idx] = mem1[_ty];
      mem2_alignment[_alignment_idx] = mem2[_tx];
    }
    _alignment_idx--;
  }

#if 0
  print_matrix(_matrix, y_matrix_len, x_matrix_len);
#endif

  // Terminate the strings so they can be printed
  mem1_alignment[longest] = 0;
  mem2_alignment[longest] = 0;

  print_diff(mem1_alignment, longest + 1, mem2_alignment, longest + 1);

  // TODO: implement me! See the bio module!
  return NULL;

}
