#include "vm.h"
#include <assert.h>

extern VM *const getVM();

void test_initVM() {
  auto vm = getVM();
  assert(vm->stack.top == 0);
}

void test_freeVM() {}

void test_interpret_add() {}

void test_interpret_subtract() {}

void test_interpret_negate() {}

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
