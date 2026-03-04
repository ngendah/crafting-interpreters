#include <assert.h>
#include <stdint.h>

#include "chunk.h"
#include "debug.h"

void test_disassemble_chunk() {
  Chunk chunk;
  initChunk(&chunk);
  writeChunk(&chunk, OP_RETURN, 123);
  const uint8_t count = disassembleChunk(&chunk, "test chunk");
  assert(count == chunk.count);
}
