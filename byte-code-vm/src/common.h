#pragma once

#include <stddef.h>
#include <stdint.h>

#define __inline(type) static inline type
#define __inline_void static inline void

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
