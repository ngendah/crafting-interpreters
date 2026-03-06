#pragma once

#include <stdint.h>

typedef unsigned int Offset;
typedef unsigned int Line;
typedef uint8_t Byte;

typedef enum {
  OP_CONSTANT = 0x01,
  OP_ADD,
  OP_SUBTRACT,
  OP_MULTIPLY,
  OP_DIVIDE,
  OP_NEGATE,
  OP_RETURN,
} OpCode;
