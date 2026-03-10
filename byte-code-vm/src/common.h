#pragma once

#include <stddef.h>
#include <stdint.h>

typedef unsigned int offset_t;
typedef unsigned int line_t;
typedef uint8_t byte_t;
typedef uint32_t hash_t;

typedef enum {
  OP_CONSTANT = 0x01,
  OP_NIL,
  OP_TRUE,
  OP_FALSE,
  OP_EQUAL,
  OP_GREATER,
  OP_LESS,
  OP_ADD,
  OP_SUBTRACT,
  OP_MULTIPLY,
  OP_DIVIDE,
  OP_NOT,
  OP_NEGATE,
  OP_RETURN,
} opcode_t;

typedef struct {
  hash_t hash;
  uint8_t is_marked;
} object_t;
