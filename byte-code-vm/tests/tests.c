#include <stdlib.h>

#include "tests.h"

int main() {
  test_init_chunk();
  test_write_chunk();
  test_disassemble_chunk();
  test_initVM();
  test_freeVM();
  test_interpret();
  test_stackInit();
  test_stackPush();
  test_stackPop();
  test_stackPeek();
  return EXIT_SUCCESS;
}
