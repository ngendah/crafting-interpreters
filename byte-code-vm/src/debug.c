#include <stdint.h>
#include <stdio.h>

#include "chunk.h"
#include "common.h"
#include "debug.h"

static offset_t debug_print_instruction(const char *name, offset_t offset);
static offset_t debug_print_constant(const char *name, chunk_t *chunk,
                                     offset_t offset);

offset_t debug_chunk(chunk_t *chunk, const char *name) {
  printf("== %s ==\n", name);
  int offset = 0;
  for (; offset < chunk->count;) {
    offset = debug_instruction(chunk, offset);
  }
  return offset;
}

offset_t debug_instruction(chunk_t *chunk, offset_t offset) {
  printf("%04d ", offset);
  if (offset > 0 && chunk->lines[offset] == chunk->lines[offset - 1]) {
    printf("|");
  } else {
    printf("%4d", chunk->lines[offset]);
  }
  const uint8_t instruction = chunk->code[offset];
  switch (instruction) {
  case OP_CONSTANT: {
    return debug_print_constant("OP_CONSTANT", chunk, offset);
  } break;
  case OP_NIL: {
    return debug_print_instruction("OP_NIL", offset);
  } break;
  case OP_TRUE: {
    return debug_print_instruction("OP_TRUE", offset);
  } break;
  case OP_FALSE: {
    return debug_print_instruction("OP_FALSE", offset);
  } break;
  case OP_ADD: {
    return debug_print_instruction("OP_ADD", offset);
  } break;
  case OP_SUBTRACT: {
    return debug_print_instruction("OP_SUBTRACT", offset);
  } break;
  case OP_MULTIPLY: {
    return debug_print_instruction("OP_MULTIPLY", offset);
  } break;
  case OP_DIVIDE: {
    return debug_print_instruction("OP_DIVIDE", offset);
  } break;
  case OP_NEGATE: {
    return debug_print_instruction("OP_NEGATE", offset);
  } break;
  case OP_NOT: {
    return debug_print_instruction("OP_NOT", offset);
  } break;
  case OP_EQUAL: {
    return debug_print_instruction("OP_EQUAL", offset);
  } break;
  case OP_GREATER: {
    return debug_print_instruction("OP_GREATER", offset);
  } break;
  case OP_LESS: {
    return debug_print_instruction("OP_LESS", offset);
  } break;
  case OP_RETURN: {
    return debug_print_instruction("OP_RETURN", offset);
  } break;
  default: {
    printf("Unknown opcode %d\n", instruction);
    return offset + 1;
  } break;
  }
}

static offset_t debug_print_instruction(const char *name, offset_t offset) {
  printf("%s\n", name);
  return offset + 1;
}

static offset_t debug_print_constant(const char *name, chunk_t *chunk,
                                     offset_t offset) {
  uint8_t constant = chunk->code[offset + 1];
  printf("%-16s %4d '", name, constant);
  value_print(chunk->constants.values[constant]);
  printf("'\n");
  return offset + 2;
}
