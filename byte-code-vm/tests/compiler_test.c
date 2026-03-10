#include <assert.h>
#include <stdlib.h>

#include "chunk.h"
#include "compiler.h"
#include "hash_table.h"
#include "value.h"

void test_compiler_eof() {
  chunk_t chunk;
  hash_table_t hash_table;
  chunk_init(&chunk);
  hash_table_init(&hash_table, value_free);
  auto result = compiler_run(_(""), &chunk, &hash_table);
  assert(result == false);
  hash_table_free(&hash_table);
  chunk_free(&chunk);
}

void test_precedence_parsing() {
  chunk_t chunk;
  hash_table_t hash_table;
  chunk_init(&chunk);
  hash_table_init(&hash_table, value_free);
  auto result = compiler_run(_("1+2+3"), &chunk, &hash_table);
  assert(result == false);
  hash_table_free(&hash_table);
  chunk_free(&chunk);
}

int main() {
  test_compiler_eof();
  test_precedence_parsing();
  return EXIT_SUCCESS;
}
