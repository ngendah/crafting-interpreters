#pragma once

#include <stdint.h>

typedef int Index;
typedef int Offset;
typedef int Line;
typedef uint8_t Byte;

typedef enum {
  OP_CONSTANT,
  OP_RETURN,
} OpCode;
