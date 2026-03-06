#include "vm.h"
#include "common.h"
#ifdef DEBUG_TRACE_EXECUTION
#include "debug.h"
#endif
#include "value.h"
#include <stdint.h>
#include <stdio.h>

#define __inline(type) static inline type
#define __inline_void static inline void

__inline(bool) isAtEnd();
__inline(InterpretResult) run();
__inline(Byte) nextInstruction();
__inline(Value) readConstant(Byte instruction);
__inline_void add();
__inline_void subtract();
__inline_void multiply();
__inline_void divide();
__inline_void negate();

VM vm;

#ifdef TESTING
VM *const initVM() {
  stackInit(&vm.stack);
  return &vm;
}
#else
void initVM() { stackInit(&vm.stack); }
#endif /* TESTING */

void freeVM() {}

InterpretResult interpret(Chunk *chunk) {
  vm.chunk = chunk;
  vm.ip = 0;
  return run();
}

__inline(InterpretResult) run() {
  while (!isAtEnd()) {
    const auto instruction = nextInstruction();
    switch (instruction) {
    case OP_CONSTANT:
      const auto constant = readConstant(instruction);
      stackPush(&vm.stack, constant);
      break;
    case OP_ADD:
      add();
      break;
    case OP_SUBTRACT:
      subtract();
      break;
    case OP_MULTIPLY:
      multiply();
      break;
    case OP_DIVIDE:
      divide();
      break;
    case OP_NEGATE:
      negate();
      break;
    case OP_RETURN: {
      printValue(stackPop(&vm.stack));
      printf("\n");
      return INTERPRET_OK;
    } break;
    default:
      break;
    }
  }
  return INTERPRET_OK;
}

__inline(bool) isAtEnd() { return (vm.ip >= vm.chunk->count); }

__inline(Byte) nextInstruction() {
  auto const ip = vm.ip;
  auto const instruction = vm.chunk->code[ip];
#ifdef DEBUG_TRACE_EXECUTION
  disassambleInstruction(vm.chunk, ip);
#endif
  vm.ip++;
  return instruction;
}

__inline(Value) readConstant(Byte instruction) {
  return vm.chunk->constants.values[instruction];
}

__inline_void add() {
  auto const rhs = stackPop(&vm.stack);
  auto const lhs = stackPop(&vm.stack);
  stackPush(&vm.stack, lhs + rhs);
}

__inline_void subtract() {
  auto const rhs = stackPop(&vm.stack);
  auto const lhs = stackPop(&vm.stack);
  stackPush(&vm.stack, lhs - rhs);
}

__inline_void multiply() {
  auto const rhs = stackPop(&vm.stack);
  auto const lhs = stackPop(&vm.stack);
  stackPush(&vm.stack, lhs * rhs);
}

__inline_void divide() {
  auto const rhs = stackPop(&vm.stack);
  auto const lhs = stackPop(&vm.stack);
  stackPush(&vm.stack, lhs / rhs);
}

__inline_void negate() { stackPush(&vm.stack, -stackPop(&vm.stack)); }

void stackInit(Stack *stack) { stack->top = 0; }

uint8_t stackPush(Stack *stack, Value value) {
  vm.stack.values[vm.stack.top++] = value;
  return vm.stack.top - 1;
}

Value stackPop(Stack *stack) { return vm.stack.values[--vm.stack.top]; }

Value stackPeek(Stack *stack) {
  auto const top = vm.stack.top - 1;
  return vm.stack.values[top];
}
