#include <stdint.h>
#include <stdio.h>

#include "chunk.h"
#include "common.h"
#include "debug.h"

static Offset simpleInstruction(const char *name, Offset offset);
static Offset constantInstruction(const char *name, Chunk *chunk,
                                  Offset offset);

Offset disassembleChunk(Chunk *chunk, const char *name) {
  printf("== %s ==\n", name);
  int offset = 0;
  for (; offset < chunk->count;) {
    offset = disassambleInstruction(chunk, offset);
  }
  return offset;
}

Offset disassambleInstruction(Chunk *chunk, Offset offset) {
  printf("%04d ", offset);
  if (offset > 0 && chunk->lines[offset] == chunk->lines[offset - 1]) {
    printf("|");
  } else {
    printf("%4d", chunk->lines[offset]);
  }
  const uint8_t instruction = chunk->code[offset];
  switch (instruction) {
  case OP_RETURN:
    return simpleInstruction("OP_RETURN", offset);
    break;
  case OP_CONSTANT:
    return constantInstruction("OP_CONSTANT", chunk, offset);
    break;
  default:
    printf("Unknown opcode %d\n", instruction);
    return offset + 1;
    break;
  }
}

static Offset simpleInstruction(const char *name, Offset offset) {
  printf("%s\n", name);
  return offset + 1;
}

static Offset constantInstruction(const char *name, Chunk *chunk,
                                  Offset offset) {
  uint8_t constant = chunk->code[offset + 1];
  printf("%-16s %4d '", name, constant);
  printValue(chunk->constants.values[constant]);
  printf("'\n");
  return offset + 2;
}
