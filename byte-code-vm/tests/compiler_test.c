#include <assert.h>
#include <stdlib.h>

#include "chunk.h"
#include "compiler.h"

void test_compiler_eof() {
  chunk_t chunk;
  chunk_init(&chunk);
  auto result = compiler_run(_(""), &chunk);
  assert(result == false);
}

void test_precedence_parsing() {
  chunk_t chunk;
  chunk_init(&chunk);
  auto result = compiler_run(_("1+2+3"), &chunk);
  assert(result == false);
}

int main() {
  test_compiler_eof();
  test_precedence_parsing();
  return EXIT_SUCCESS;
}
