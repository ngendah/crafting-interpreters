#include "chunk.h"
#include "memory.h"
#include "value.h"

void chunk_write(chunk_t *chunk, byte_t byte, line_t line) {
  if (chunk->count + 1 >= chunk->capacity) {
    size_t len = chunk->capacity * GROW_RATE;
    // BUG:
    chunk->code = resize(chunk->code, len * sizeof(byte_t));
    chunk->lines = resize(chunk->lines, len * sizeof(line_t));
    chunk->capacity = len;
  }
  chunk->code[chunk->count] = byte;
  chunk->lines[chunk->count] = line;
  chunk->count++;
}

offset_t chunk_add_constant(chunk_t *chunk, value_t value) {
  values_write(&chunk->constants, value);
  return chunk->constants.count - 1;
}

void chunk_init(chunk_t *const chunk) {
  chunk->count = 0;
  chunk->capacity = MIN_ARRAY_LEN;
  chunk->code = (byte_t *)new(chunk->capacity * sizeof(byte_t));
  chunk->lines = (line_t *)new(chunk->capacity * sizeof(line_t));
  values_init(&chunk->constants);
}

void chunk_free(chunk_t *const chunk) {
  delete(chunk->code);
  delete(chunk->lines);
  values_free(&chunk->constants);
  chunk->count = 0;
  chunk->capacity = 0;
  chunk->code = nullptr;
  chunk->lines = nullptr;
}
