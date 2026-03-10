#include <assert.h>
#include <stdlib.h>

#include "chunk.h"
#include "common.h"
#include "vm.h"

extern vm_t vm;
extern bool vm_is_end();
extern byte_t vm_next_instruction();
extern vm_result_t vm_execute();
extern void vm_add();
extern void vm_subtract();

void test_vm_is_end() {
  vm_init();
  {
    assert(vm_is_end() == true);
  }
  vm_free();
}

void test_vm_next_instruction() {
  vm_init();
  {
    chunk_write(vm.chunk, OP_ADD, 1);
    byte_t instruction = vm_next_instruction();
    assert(instruction == OP_ADD);
    instruction = vm_next_instruction();
    assert(instruction == '\0');
  }
  vm_free();
}

void test_add_operation() {
  vm_init();
  {
    stack_push(&vm.stack, value_from_number(1));
    stack_push(&vm.stack, value_from_number(2));
    chunk_write(vm.chunk, OP_ADD, 1);
    vm_add();
    value_t value = stack_peek(&vm.stack);
    assert(value_as_number(value) == 3);
  }
  vm_free();
}

void test_subtract_operation() {
  vm_init();
  {
    chunk_write(vm.chunk, OP_CONSTANT, 1);
    offset_t offset = chunk_add_constant(vm.chunk, value_from_number(5));
    chunk_write(vm.chunk, offset, 1);
    chunk_write(vm.chunk, OP_CONSTANT, 1);
    offset = chunk_add_constant(vm.chunk, value_from_number(2));
    chunk_write(vm.chunk, offset, 1);
    chunk_write(vm.chunk, OP_SUBTRACT, 1);
    vm_execute();
    value_t value = stack_peek(&vm.stack);
    assert(value_as_number(value) == 3);
  }
  vm_free();
}

int main() {
  test_vm_is_end();
  test_vm_next_instruction();
  test_add_operation();
  test_subtract_operation();
  return EXIT_SUCCESS;
}
