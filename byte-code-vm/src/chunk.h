#pragma once

#include "common.h"
#include "value.h"
#include <stddef.h>
#include <stdint.h>

typedef struct {
  size_t count;
  size_t capacity;
  Byte *code; // :)
  Line *lines;
  Values constants;
} Chunk;

void initChunk(Chunk *chunk);
void freeChunk(Chunk *chunk);
void writeChunk(Chunk *chunk, Byte byte, Line line);
Offset addConstant(Chunk *chunk, Value value);
