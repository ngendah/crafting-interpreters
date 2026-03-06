#pragma once

#include "chunk.h"
#include "common.h"
#include "value.h"
#include <stdint.h>

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

#ifdef TESTING
VM *const initVM();
#else
void initVM();
#endif // TESTING
void freeVM();
InterpretResult interpret(Chunk *chunk);

void stackInit(Stack *stack);
uint8_t stackPush(Stack *stack, Value value);
Value stackPop(Stack *stack);
Value stackPeek(Stack *stack);
