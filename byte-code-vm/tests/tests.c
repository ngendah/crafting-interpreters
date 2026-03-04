#include <stdlib.h>

#include "tests.h"

int main() {
  test_init_chunk();
  test_write_chunk();
  test_disassemble_chunk();
  return EXIT_SUCCESS;
}
