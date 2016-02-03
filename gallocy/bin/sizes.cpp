#include <cstdint>
#include <cstdlib>
#include <iostream>

#define PRINT_SIZE(type) std::cout << #type << ": " << sizeof(type) << std::endl

struct a_struct {
  uint64_t dummy;
};

struct recursive_struct {
  recursive_struct *dummy;
};

class a_class {
  uint64_t dummy;
};

int main(int argc, char *argv[]) {
  PRINT_SIZE(double *);
  PRINT_SIZE(double);
  PRINT_SIZE(size_t);
  PRINT_SIZE(uint32_t *);
  PRINT_SIZE(uint32_t);
  PRINT_SIZE(uint64_t *);
  PRINT_SIZE(uint64_t);
  PRINT_SIZE(void *);

  std::cout << "struct { uint64_t }: " << sizeof(a_struct) << std::endl;
  std::cout << "struct { struct }: " << sizeof(recursive_struct) << std::endl;
  std::cout << "class { uint64_t }: " << sizeof(a_class) << std::endl;
}
