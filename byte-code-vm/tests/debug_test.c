#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include "chunk.h"
#include "debug.h"

void test_debug_chunk() {
  chunk_t chunk;
  chunk_init(&chunk);
  chunk_write(&chunk, OP_RETURN, 123);
  const uint8_t count = debug_chunk(&chunk, "test chunk");
  assert(count == chunk.count);
}

int main() {
  test_debug_chunk();
  return EXIT_SUCCESS;
}
