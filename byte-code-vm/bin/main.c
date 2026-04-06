#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chunk.h"
#include "compiler.h"
#include "environment.h"
#include "vm.h"

void run_compiler(const string_t source) {
  chunk_t chunk;
  environment_t environment;

  chunk_init(&chunk);
  environment_init(&environment);

  auto result = compiler_run(source, &chunk, &environment);

  environment_free(&environment);
  chunk_free(&chunk);
}

void run_vm(const string_t source) {
  vm_init();
  vm_run(source);
  vm_free();
}

int main(int argc, char *argv[argc + 1]) {
  constexpr auto buffer_len = 1024u;
  char buffer[buffer_len] = {};
  snprintf(buffer, buffer_len, "%s%s", argv[0], ".lox");
  const auto file = fopen(buffer, "r");
  if (file) {
    memset(buffer, 0, buffer_len);
    const auto len = fread(buffer, sizeof(char), buffer_len, file);
#ifdef DEBUG_ENABLED
    printf("\n%.*s\n", (int)len, buffer);
#endif
    run_vm(_T(buffer, len));
    return EXIT_SUCCESS;
  }
  return EXIT_FAILURE;
}
