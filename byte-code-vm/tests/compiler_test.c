#include <assert.h>
#include <stdlib.h>

#include "chunk.h"
#include "compiler.h"
#include "environment.h"

void test_compiler_eof() {
  chunk_t chunk;
  environment_t environment;

  chunk_init(&chunk);
  environment_init(&environment);

  auto result = compiler_run(_(""), &chunk, &environment);
  // assert(result == false);
  environment_free(&environment);
  chunk_free(&chunk);
}

void test_precedence_parsing() {
  chunk_t chunk;
  environment_t environment;

  chunk_init(&chunk);
  environment_init(&environment);

  auto result = compiler_run(_("1+2+3"), &chunk, &environment);
  // assert(result == false);
  environment_free(&environment);
  chunk_free(&chunk);
}

int main() {
  // test_compiler_eof();
  // test_precedence_parsing();
  return EXIT_SUCCESS;
}
