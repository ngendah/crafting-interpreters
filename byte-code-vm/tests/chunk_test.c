#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

#include "chunk.h"
#include "memory.h"

void test_init_chunk() {
  chunk_t chunk;
  chunk_init(&chunk);
  assert(chunk.capacity == MIN_ARRAY_LEN);
  assert(chunk.count == 0);
}

void test_write_chunk() {
  chunk_t chunk;
  chunk_init(&chunk);
  chunk_write(&chunk, OP_RETURN, 123);
  assert(chunk.count == 1);
  assert(chunk.code[0] == OP_RETURN);
  assert(chunk.capacity == 8);
  opcode_t codes[] = {
      OP_ADD, OP_SUBTRACT, OP_MULTIPLY, OP_DIVIDE, OP_NEGATE, OP_RETURN,
      OP_ADD, OP_SUBTRACT, OP_MULTIPLY, OP_DIVIDE, OP_NEGATE, OP_RETURN,
  };
  auto len = (sizeof codes / sizeof codes[0]);
  for (auto index = 0; index < len; index++) {
    chunk_write(&chunk, codes[index], 123);
  }
  assert(chunk.count == 1 + len);
  assert(chunk.capacity == 16);
}

void test_free_chunk() {
  chunk_t chunk;
  chunk_init(&chunk);
  chunk_write(&chunk, OP_RETURN, 123);
  assert(chunk.count == 1);
  assert(chunk.code[0] == OP_RETURN);
  assert(chunk.capacity == 8);
  chunk_free(&chunk);
  assert(chunk.count == 0);
  assert(chunk.capacity == 0);
  assert(chunk.code == NULL);
}

int main() {
  test_init_chunk();
  test_write_chunk();
  test_free_chunk();
  return EXIT_SUCCESS;
}
