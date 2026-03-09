#pragma once

#include <stddef.h>
#include <stdint.h>

#include "chunk.h"
#include "common.h"
#include "string.h"
#include "value.h"

typedef enum {
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR,
} InterpretResult;

typedef struct {
  Value values[256];
  uint8_t top;
} Stack;

typedef struct {
  Chunk *chunk;
  Offset ip;
  Stack stack;
} VM;

void initVM();
void freeVM();
InterpretResult interpret(const String source);

void stackInit(Stack *stack);
uint8_t stackPush(Stack *stack, Value value);
Value stackPop(Stack *stack);
Value stackPeek(Stack *stack);
