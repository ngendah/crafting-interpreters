#pragma once

#include <stddef.h>
#include <stdint.h>

typedef unsigned int offset_t;
typedef unsigned int line_t;
typedef uint8_t byte_t;
typedef uint64_t hash_t;

typedef enum : byte_t {
  OP_ERROR = 0x00,
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
  OP_PRINT,
  OP_POP,
  OP_VARIABLE_DEFINE,
  OP_VARIABLE_GET,
  OP_VARIABLE_SET,
  OP_JUMP,
  OP_JUMP_IF_FALSE,
  OP_LOOP,
} opcode_t;

static const char *opcode_as_string[] = {
    [OP_ERROR] = "",
    [OP_CONSTANT] = "OP_CONSTANT",
    [OP_NIL] = "OP_NIL",
    [OP_TRUE] = "OP_TRUE",
    [OP_FALSE] = "OP_FALSE",
    [OP_EQUAL] = "OP_EQUAL",
    [OP_GREATER] = "OP_GREATER",
    [OP_LESS] = "OP_LESS",
    [OP_ADD] = "OP_ADD",
    [OP_SUBTRACT] = "OP_SUBTRACT",
    [OP_MULTIPLY] = "OP_MULTIPLY",
    [OP_DIVIDE] = "OP_DIVIDE",
    [OP_NOT] = "OP_NOT",
    [OP_NEGATE] = "OP_NEGATE",
    [OP_RETURN] = "OP_RETURN",
    [OP_PRINT] = "OP_PRINT",
    [OP_POP] = "OP_POP",
    [OP_VARIABLE_DEFINE] = "OP_VARIABLE_DEFINE",
    [OP_VARIABLE_GET] = "OP_VARIABLE_GET",
    [OP_VARIABLE_SET] = "OP_VARIABLE_SET",
    [OP_JUMP] = "OP_JUMP",
    [OP_JUMP_IF_FALSE] = "OP_JUMP_IF_FALSE",
    [OP_LOOP] = "OP_LOOP",
};

typedef struct {
  hash_t hash;
  bool is_marked;
} object_t;
