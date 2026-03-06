#include "chunk.h"
#include "common.h"
#include "vm.h"
#include <assert.h>

void test_initVM() {
  auto vm = initVM();
  assert(vm->stack.top == 0);
}

void test_freeVM() {}

void test_interpret_add() {
  Chunk chunk;
  initChunk(&chunk);
  writeChunk(&chunk, OP_ADD, 0);
  VM *vm = initVM();
  stackPush(&vm->stack, 1);
  stackPush(&vm->stack, 2);
  interpret(&chunk);
  assert(stackPeek(&vm->stack) == 3);
}

void test_interpret_subtract() {
  Chunk chunk;
  initChunk(&chunk);
  writeChunk(&chunk, OP_SUBTRACT, 0);
  VM *vm = initVM();
  stackPush(&vm->stack, 1);
  stackPush(&vm->stack, 2);
  interpret(&chunk);
  assert(stackPeek(&vm->stack) == -1);
}

void test_interpret_negate() {
  Chunk chunk;
  initChunk(&chunk);
  writeChunk(&chunk, OP_NEGATE, 0);
  VM *vm = initVM();
  stackPush(&vm->stack, -2);
  interpret(&chunk);
  assert(stackPeek(&vm->stack) == 2);
}

void test_interpret() {
  test_interpret_add();
  test_interpret_subtract();
  test_interpret_negate();
}
void test_stackInit() {}
void test_stackPush() {}
void test_stackPop() {}
void test_stackPeek() {}

int main() {
  test_initVM();
  test_freeVM();
  test_interpret();
  test_stackInit();
  test_stackPush();
  test_stackPop();
  test_stackPeek();
}
