#include "chunk.h"
#include "memory.h"
#include "value.h"

void initChunk(Chunk *chunk) {
  chunk->count = 0;
  chunk->capacity = 0;
  chunk->code = NULL;
  chunk->lines = NULL;
  initValues(&chunk->constants);
}

void freeChunk(Chunk *chunk) {
  FREE_ARRAY(Byte, chunk->code, chunk->capacity);
  FREE_ARRAY(Line, chunk->lines, chunk->capacity);
  freeValues(&chunk->constants);
  initChunk(chunk);
}

void writeChunk(Chunk *chunk, Byte byte, Line line) {
  if (chunk->capacity < chunk->count + 1) {
    int oldCapacity = chunk->capacity;
    chunk->capacity = GROW_CAPACITY(oldCapacity);
    chunk->code = GROW_ARRAY(Byte, chunk->code, oldCapacity, chunk->capacity);
    chunk->lines = GROW_ARRAY(Line, chunk->lines, oldCapacity, chunk->capacity);
  }
  chunk->code[chunk->count] = byte;
  chunk->lines[chunk->count] = line;
  chunk->count++;
}

Index addConstant(Chunk *chunk, Value value) {
  writeValue(&chunk->constants, value);
  return chunk->constants.count - 1;
}
