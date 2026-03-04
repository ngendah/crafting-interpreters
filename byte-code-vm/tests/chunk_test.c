#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

#include "chunk.h"

void test_init_chunk() {
  Chunk chunk;
  initChunk(&chunk);
  assert(chunk.capacity == 0);
  assert(chunk.code == NULL);
  assert(chunk.count == 0);
}

void test_write_chunk() {
  Chunk chunk;
  initChunk(&chunk);
  writeChunk(&chunk, OP_RETURN, 123);
  assert(chunk.count == 1);
  assert(chunk.code[0] == OP_RETURN);
  assert(chunk.capacity == 8);
}

void test_free_chunk() {
  Chunk chunk;
  initChunk(&chunk);
  writeChunk(&chunk, OP_RETURN, 123);
  assert(chunk.count == 1);
  assert(chunk.code[0] == OP_RETURN);
  assert(chunk.capacity == 8);
  freeChunk(&chunk);
  assert(chunk.count == 0);
  assert(chunk.capacity == 0);
  assert(chunk.code == NULL);
}
