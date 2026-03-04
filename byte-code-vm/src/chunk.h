#pragma once

#include "common.h"
#include "value.h"
#include <stdint.h>

typedef struct {
  int count;
  int capacity;
  Byte *code; // :)
  Line *lines;
  Values constants;
} Chunk;

void initChunk(Chunk *chunk);
void freeChunk(Chunk *chunk);
void writeChunk(Chunk *chunk, Byte byte, Line line);
Index addConstant(Chunk *chunk, Value value);
